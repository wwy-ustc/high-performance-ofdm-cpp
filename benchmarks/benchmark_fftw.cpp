#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>

#include <fftw3.h>

#include "ofdm.hpp"


struct Result
{
    double avg;
    double p50;
    double p95;
};


Result summarize(std::vector<double> times)
{
    std::sort(times.begin(), times.end());

    double sum = 0.0;

    for (double t : times)
    {
        sum += t;
    }

    Result result;

    result.avg =
        sum / static_cast<double>(times.size());

    result.p50 =
        times[
            static_cast<std::size_t>(
                0.50 * (times.size() - 1)
            )
        ];

    result.p95 =
        times[
            static_cast<std::size_t>(
                0.95 * (times.size() - 1)
            )
        ];

    return result;
}


// ============================================================
// Legacy IFFT
//
// 每次调用都完整执行：
//
// allocate buffer
// -> copy input
// -> create plan
// -> execute
// -> copy + normalize output
// -> destroy plan
// -> free buffer
//
// 与 OfdmEngine::modulate() 保持相同计算语义。
// ============================================================

std::vector<std::complex<double>>
legacyIFFT(
    const std::vector<double>& symbols
)
{
    int N =
        static_cast<int>(
            symbols.size()
        );


    fftw_complex* input =
        fftw_alloc_complex(N);

    fftw_complex* output =
        fftw_alloc_complex(N);


    if (input == nullptr ||
        output == nullptr)
    {
        throw std::runtime_error(
            "Failed to allocate FFTW buffers."
        );
    }


    // 输入拷贝
    for (int i = 0; i < N; ++i)
    {
        input[i][0] = symbols[i];
        input[i][1] = 0.0;
    }


    // 每次重新创建 Plan
    fftw_plan plan =
        fftw_plan_dft_1d(
            N,
            input,
            output,
            FFTW_BACKWARD,
            FFTW_ESTIMATE
        );


    if (plan == nullptr)
    {
        fftw_free(input);
        fftw_free(output);

        throw std::runtime_error(
            "Failed to create FFTW plan."
        );
    }


    // 执行 IFFT
    fftw_execute(plan);


    // 输出拷贝 + 归一化
    std::vector<std::complex<double>>
        result(N);


    for (int i = 0; i < N; ++i)
    {
        result[i] =
            std::complex<double>(
                output[i][0] / N,
                output[i][1] / N
            );
    }


    // 释放资源
    fftw_destroy_plan(plan);

    fftw_free(input);
    fftw_free(output);


    return result;
}


// ============================================================
// 单个 workload 的 A/B Benchmark
// ============================================================

void runBenchmark(
    int N,
    int warmupIterations,
    int measuredIterations
)
{
    // --------------------------------------------------------
    // 固定输入
    // --------------------------------------------------------

    std::vector<double> symbols(N);

    std::mt19937 generator(42);

    std::uniform_int_distribution<int>
        distribution(0, 1);


    for (int i = 0; i < N; ++i)
    {
        symbols[i] =
            distribution(generator)
            ? 1.0
            : -1.0;
    }


    // --------------------------------------------------------
    // Optimized Engine
    //
    // Buffer 和 Plan 在这里初始化一次
    // --------------------------------------------------------

    OfdmEngine engine(N);


    std::vector<std::complex<double>>
        legacyOutput;

    std::vector<std::complex<double>>
        optimizedOutput;


    // --------------------------------------------------------
    // Warm-up
    // --------------------------------------------------------

    for (int i = 0;
         i < warmupIterations;
         ++i)
    {
        legacyOutput =
            legacyIFFT(symbols);

        optimizedOutput =
            engine.modulate(symbols);
    }


    // --------------------------------------------------------
    // 正式测量
    //
    // 偶数轮：
    // Legacy -> Optimized
    //
    // 奇数轮：
    // Optimized -> Legacy
    //
    // 减少固定执行顺序造成的系统偏差
    // --------------------------------------------------------

    std::vector<double> legacyTimes;

    std::vector<double> optimizedTimes;


    legacyTimes.reserve(
        measuredIterations
    );

    optimizedTimes.reserve(
        measuredIterations
    );


    for (int i = 0;
         i < measuredIterations;
         ++i)
    {
        if (i % 2 == 0)
        {
            // Legacy
            auto legacyStart =
                std::chrono::steady_clock::now();


            legacyOutput =
                legacyIFFT(symbols);


            auto legacyEnd =
                std::chrono::steady_clock::now();


            std::chrono::duration<
                double,
                std::milli
            > legacyElapsed =
                legacyEnd - legacyStart;


            legacyTimes.push_back(
                legacyElapsed.count()
            );


            // Optimized
            auto optimizedStart =
                std::chrono::steady_clock::now();


            optimizedOutput =
                engine.modulate(symbols);


            auto optimizedEnd =
                std::chrono::steady_clock::now();


            std::chrono::duration<
                double,
                std::milli
            > optimizedElapsed =
                optimizedEnd -
                optimizedStart;


            optimizedTimes.push_back(
                optimizedElapsed.count()
            );
        }
        else
        {
            // Optimized
            auto optimizedStart =
                std::chrono::steady_clock::now();


            optimizedOutput =
                engine.modulate(symbols);


            auto optimizedEnd =
                std::chrono::steady_clock::now();


            std::chrono::duration<
                double,
                std::milli
            > optimizedElapsed =
                optimizedEnd -
                optimizedStart;


            optimizedTimes.push_back(
                optimizedElapsed.count()
            );


            // Legacy
            auto legacyStart =
                std::chrono::steady_clock::now();


            legacyOutput =
                legacyIFFT(symbols);


            auto legacyEnd =
                std::chrono::steady_clock::now();


            std::chrono::duration<
                double,
                std::milli
            > legacyElapsed =
                legacyEnd - legacyStart;


            legacyTimes.push_back(
                legacyElapsed.count()
            );
        }
    }


    // --------------------------------------------------------
    // 正确性检查
    // --------------------------------------------------------

    double maxDifference = 0.0;


    for (int i = 0; i < N; ++i)
    {
        double difference =
            std::abs(
                legacyOutput[i]
                -
                optimizedOutput[i]
            );


        if (difference > maxDifference)
        {
            maxDifference =
                difference;
        }
    }


    // --------------------------------------------------------
    // 统计
    // --------------------------------------------------------

    Result legacyResult =
        summarize(legacyTimes);


    Result optimizedResult =
        summarize(optimizedTimes);


    double speedup =
        legacyResult.p50
        /
        optimizedResult.p50;


    double reduction =
        (
            legacyResult.p50
            -
            optimizedResult.p50
        )
        /
        legacyResult.p50
        * 100.0;


    // --------------------------------------------------------
    // 输出一行
    // --------------------------------------------------------

    std::cout
        << std::left
        << std::setw(12)
        << N

        << std::setw(16)
        << legacyResult.p50

        << std::setw(18)
        << optimizedResult.p50

        << std::setw(12)
        << speedup

        << std::setw(16)
        << reduction

        << maxDifference

        << "\n";
}


// ============================================================
// main
// ============================================================

int main()
{
    std::cout
        << "============================================================\n";

    std::cout
        << "FFTW IFFT Workload Scaling Benchmark\n";

    std::cout
        << "============================================================\n";


    std::cout
        << std::left
        << std::setw(12)
        << "Size"

        << std::setw(16)
        << "Legacy P50"

        << std::setw(18)
        << "Optimized P50"

        << std::setw(12)
        << "Speedup"

        << std::setw(16)
        << "Reduction(%)"

        << "Max Error"

        << "\n";


    std::cout
        << "------------------------------------------------------------"
        << "----------------\n";


    // 小 workload 多测一些
    runBenchmark(
    1024,       // 2^10
    10,
    200
);

runBenchmark(
    16384,      // 2^14
    10,
    200
);

runBenchmark(
    131072,     // 2^17
    10,
    100
);

runBenchmark(
    1048576,    // 2^20
    5,
    30
);

    return 0;
}