#include <algorithm>
#include <chrono>
#include <complex>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include <fftw3.h>


struct BenchmarkResult
{
    double avg;
    double p50;
    double p95;
};


BenchmarkResult summarize(std::vector<double> times)
{
    std::sort(times.begin(), times.end());

    double sum = 0.0;

    for (double value : times)
    {
        sum += value;
    }

    BenchmarkResult result;

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
// ThreadedIFFT
//
// 每个对象：
// - 分配自己的 FFTW Buffer
// - 按指定线程数创建 Plan
// - 后续重复执行 Plan
// ============================================================

class ThreadedIFFT
{
public:

    ThreadedIFFT(
        int size,
        int threadCount
    )
        : size_(size),
          threadCount_(threadCount)
    {
        input_ =
            fftw_alloc_complex(size_);

        output_ =
            fftw_alloc_complex(size_);


        if (input_ == nullptr ||
            output_ == nullptr)
        {
            throw std::runtime_error(
                "Failed to allocate FFTW buffers."
            );
        }


        // 设置“接下来创建的 Plan”使用多少线程
        fftw_plan_with_nthreads(
            threadCount_
        );


        plan_ =
            fftw_plan_dft_1d(
                size_,
                input_,
                output_,
                FFTW_BACKWARD,
                FFTW_ESTIMATE
            );


        if (plan_ == nullptr)
        {
            throw std::runtime_error(
                "Failed to create FFTW plan."
            );
        }
    }


    ~ThreadedIFFT()
    {
        fftw_destroy_plan(plan_);

        fftw_free(input_);
        fftw_free(output_);
    }


    ThreadedIFFT(
        const ThreadedIFFT&
    ) = delete;


    ThreadedIFFT& operator=(
        const ThreadedIFFT&
    ) = delete;


    void execute(
        const std::vector<double>& symbols
    )
    {
        for (int i = 0; i < size_; ++i)
        {
            input_[i][0] =
                symbols[i];

            input_[i][1] =
                0.0;
        }


        fftw_execute(plan_);


        // 为了与真实 pipeline 更接近，
        // 保留输出读取与归一化过程。
        for (int i = 0; i < size_; ++i)
        {
            sink_ +=
                output_[i][0]
                /
                static_cast<double>(size_);
        }
    }


private:

    int size_;

    int threadCount_;

    fftw_complex* input_;

    fftw_complex* output_;

    fftw_plan plan_;

    // 防止编译器完全忽略输出
    volatile double sink_ = 0.0;
};


// ============================================================
// 单一 workload / thread count Benchmark
// ============================================================

BenchmarkResult runBenchmark(
    int size,
    int threadCount,
    int warmupIterations,
    int measuredIterations
)
{
    std::vector<double> symbols(size);


    std::mt19937 generator(42);

    std::uniform_int_distribution<int>
        distribution(0, 1);


    for (int i = 0; i < size; ++i)
    {
        symbols[i] =
            distribution(generator)
            ? 1.0
            : -1.0;
    }


    ThreadedIFFT engine(
        size,
        threadCount
    );


    // Warm-up
    for (int i = 0;
         i < warmupIterations;
         ++i)
    {
        engine.execute(symbols);
    }


    std::vector<double> times;

    times.reserve(
        measuredIterations
    );


    for (int i = 0;
         i < measuredIterations;
         ++i)
    {
        auto start =
            std::chrono::steady_clock::now();


        engine.execute(symbols);


        auto end =
            std::chrono::steady_clock::now();


        std::chrono::duration<
            double,
            std::milli
        > elapsed =
            end - start;


        times.push_back(
            elapsed.count()
        );
    }


    return summarize(times);
}


// ============================================================
// main
// ============================================================

int main()
{
    // FFTW 多线程模块初始化
    if (fftw_init_threads() == 0)
    {
        std::cerr
            << "Failed to initialize FFTW threads.\n";

        return 1;
    }


    unsigned int hardwareThreads =
        std::thread::hardware_concurrency();


    std::cout
        << "============================================\n";

    std::cout
        << "FFTW Multi-thread Scaling Benchmark\n";

    std::cout
        << "============================================\n";

    std::cout
        << "Detected logical CPUs: "
        << hardwareThreads
        << "\n\n";


    std::vector<int> threadCounts =
    {
        1,
        2,
        4,
        8
    };


    // 主要测试大 workload
    std::vector<int> workloadSizes =
    {
        131072,     // 2^17
        1048576     // 2^20
    };


    for (int size : workloadSizes)
    {
        std::cout
            << "--------------------------------------------\n";

        std::cout
            << "IFFT Size: "
            << size
            << "\n";

        std::cout
            << "--------------------------------------------\n";


        std::cout
            << std::left
            << std::setw(10)
            << "Threads"

            << std::setw(16)
            << "P50(ms)"

            << std::setw(16)
            << "P95(ms)"

            << std::setw(16)
            << "Speedup"

            << "\n";


        double baselineP50 = 0.0;


        for (int threadCount : threadCounts)
        {
            if (
                hardwareThreads != 0
                &&
                static_cast<unsigned int>(
                    threadCount
                ) > hardwareThreads
            )
            {
                continue;
            }


            int warmupIterations;

            int measuredIterations;


            if (size == 131072)
            {
                warmupIterations = 10;

                measuredIterations = 100;
            }
            else
            {
                warmupIterations = 5;

                measuredIterations = 30;
            }


            BenchmarkResult result =
                runBenchmark(
                    size,
                    threadCount,
                    warmupIterations,
                    measuredIterations
                );


            if (threadCount == 1)
            {
                baselineP50 =
                    result.p50;
            }


            double speedup =
                baselineP50
                /
                result.p50;


            std::cout
                << std::left
                << std::setw(10)
                << threadCount

                << std::setw(16)
                << result.p50

                << std::setw(16)
                << result.p95

                << std::setw(16)
                << speedup

                << "\n";
        }


        std::cout << "\n";
    }


    fftw_cleanup_threads();


    return 0;
}