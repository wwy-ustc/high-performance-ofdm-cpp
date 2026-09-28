#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <chrono>
#include <algorithm>

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


void printResult(
    const std::string& name,
    const Result& result
)
{
    std::cout << name << "\n";

    std::cout
        << "  avg : "
        << result.avg
        << " ms\n";

    std::cout
        << "  p50 : "
        << result.p50
        << " ms\n";

    std::cout
        << "  p95 : "
        << result.p95
        << " ms\n\n";
}


// ============================================================
// Legacy IFFT
//
// 每次调用都：
// allocate
// create plan
// execute
// destroy
// free
// ============================================================

void legacyIFFT(
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


    for (int i = 0; i < N; ++i)
    {
        input[i][0] = symbols[i];
        input[i][1] = 0.0;
    }


    fftw_plan plan =
        fftw_plan_dft_1d(
            N,
            input,
            output,
            FFTW_BACKWARD,
            FFTW_ESTIMATE
        );


    fftw_execute(plan);


    fftw_destroy_plan(plan);

    fftw_free(input);

    fftw_free(output);
}


int main()
{
    const int N = 100000;

    const int warmupIterations = 10;

    const int measuredIterations = 100;


    // ========================================================
    // 固定输入
    // ========================================================

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


    // ========================================================
    // Optimized Engine
    // ========================================================

    OfdmEngine engine(N);


    // ========================================================
    // Warm-up
    // ========================================================

    for (int i = 0;
         i < warmupIterations;
         ++i)
    {
        legacyIFFT(symbols);

        engine.modulate(symbols);
    }


    // ========================================================
    // A/B Benchmark
    //
    // 每轮交替测试：
    //
    // 偶数轮：
    // Legacy -> Optimized
    //
    // 奇数轮：
    // Optimized -> Legacy
    //
    // 避免固定测试顺序带来的偏差
    // ========================================================

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
            auto startLegacy =
                std::chrono::steady_clock::now();

            legacyIFFT(symbols);

            auto endLegacy =
                std::chrono::steady_clock::now();


            std::chrono::duration<double, std::milli>
                legacyElapsed =
                    endLegacy - startLegacy;


            legacyTimes.push_back(
                legacyElapsed.count()
            );


            // Optimized
            auto startOptimized =
                std::chrono::steady_clock::now();

            engine.modulate(symbols);

            auto endOptimized =
                std::chrono::steady_clock::now();


            std::chrono::duration<double, std::milli>
                optimizedElapsed =
                    endOptimized - startOptimized;


            optimizedTimes.push_back(
                optimizedElapsed.count()
            );
        }
        else
        {
            // Optimized
            auto startOptimized =
                std::chrono::steady_clock::now();

            engine.modulate(symbols);

            auto endOptimized =
                std::chrono::steady_clock::now();


            std::chrono::duration<double, std::milli>
                optimizedElapsed =
                    endOptimized - startOptimized;


            optimizedTimes.push_back(
                optimizedElapsed.count()
            );


            // Legacy
            auto startLegacy =
                std::chrono::steady_clock::now();

            legacyIFFT(symbols);

            auto endLegacy =
                std::chrono::steady_clock::now();


            std::chrono::duration<double, std::milli>
                legacyElapsed =
                    endLegacy - startLegacy;


            legacyTimes.push_back(
                legacyElapsed.count()
            );
        }
    }


    // ========================================================
    // 统计
    // ========================================================

    Result legacyResult =
        summarize(
            legacyTimes
        );


    Result optimizedResult =
        summarize(
            optimizedTimes
        );


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


    // ========================================================
    // 输出
    // ========================================================

    std::cout
        << "====================================\n";

    std::cout
        << "FFTW IFFT Microbenchmark\n";

    std::cout
        << "====================================\n";

    std::cout
        << "Data size: "
        << N
        << "\n";

    std::cout
        << "Warm-up iterations: "
        << warmupIterations
        << "\n";

    std::cout
        << "Measured iterations: "
        << measuredIterations
        << "\n";

    std::cout
        << "====================================\n\n";


    printResult(
        "Legacy IFFT",
        legacyResult
    );


    printResult(
        "Optimized IFFT",
        optimizedResult
    );


    std::cout
        << "P50 Speedup: "
        << speedup
        << "x\n";


    std::cout
        << "P50 Latency Reduction: "
        << reduction
        << "%\n";


    return 0;
}