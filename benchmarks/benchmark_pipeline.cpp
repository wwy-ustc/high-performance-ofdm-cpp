#include <iostream>
#include <vector>
#include <complex>
#include <random>
#include <iomanip>
#include <string>

#include "bpsk.hpp"
#include "logistic.hpp"
#include "encryption.hpp"
#include "ofdm.hpp"
#include "benchmark.hpp"


// ============================================================
// 一次完整 Pipeline Benchmark 的结果
// ============================================================

struct PipelineResult
{
    BenchmarkResult performance;

    double throughput;

    bool correctness;
};


// ============================================================
// 测试某一个 workload + 某一个线程数
// ============================================================

PipelineResult runPipelineBenchmark(
    int N,
    int threadCount,
    int warmupIterations,
    int measuredIterations
)
{
    // --------------------------------------------------------
    // 固定输入
    // --------------------------------------------------------

    std::vector<int> bits(N);

    std::mt19937 generator(42);

    std::uniform_int_distribution<int>
        distribution(0, 1);


    for (int i = 0; i < N; ++i)
    {
        bits[i] =
            distribution(generator);
    }


    // --------------------------------------------------------
    // Pipeline 中间结果
    // --------------------------------------------------------

    std::vector<double> symbols;

    std::vector<double> chaos;

    std::vector<double> encrypted;

    std::vector<std::complex<double>>
        timeSamples;

    std::vector<std::complex<double>>
        recovered;

    std::vector<double>
        recoveredEncrypted;

    std::vector<double>
        decrypted;

    std::vector<int>
        recoveredBits;


    // --------------------------------------------------------
    // 创建指定线程数的 OFDM Engine
    //
    // 注意：
    // Engine 初始化不计入运行阶段 Benchmark
    // --------------------------------------------------------

    OfdmEngine ofdm(
        N,
        threadCount
    );


    // --------------------------------------------------------
    // End-to-End Benchmark
    // --------------------------------------------------------

    BenchmarkResult result =
        benchmark(
            [&]()
            {
                // 1. BPSK
                symbols =
                    bpskModulate(bits);


                // 2. Logistic
                chaos =
                    generateLogisticSequence(
                        N,
                        0.54321,
                        3.999
                    );


                // 3. Encryption
                encrypted =
                    encryptBPSK(
                        symbols,
                        chaos
                    );


                // 4. IFFT
                timeSamples =
                    ofdm.modulate(
                        encrypted
                    );


                // 5. FFT
                recovered =
                    ofdm.demodulate(
                        timeSamples
                    );


                // 6. Complex -> Real
                recoveredEncrypted.resize(N);

                for (int i = 0; i < N; ++i)
                {
                    recoveredEncrypted[i] =
                        recovered[i].real();
                }


                // 7. Decryption
                decrypted =
                    decryptBPSK(
                        recoveredEncrypted,
                        chaos
                    );


                // 8. BPSK Demodulation
                recoveredBits =
                    bpskDemodulate(
                        decrypted
                    );
            },
            warmupIterations,
            measuredIterations
        );


    // --------------------------------------------------------
    // Throughput
    //
    // p50Ms 是毫秒
    // 除以1000转换成秒
    // --------------------------------------------------------

    double throughput =
        static_cast<double>(N)
        /
        (result.p50Ms / 1000.0);


    // --------------------------------------------------------
    // End-to-End 正确性
    // --------------------------------------------------------

    bool correctness =
        (bits == recoveredBits);


    PipelineResult pipelineResult;

    pipelineResult.performance =
        result;

    pipelineResult.throughput =
        throughput;

    pipelineResult.correctness =
        correctness;


    return pipelineResult;
}


// ============================================================
// 测试某一个 workload 的 1/2/4/8 Thread Scaling
// ============================================================

void runThreadScaling(
    int N,
    int warmupIterations,
    int measuredIterations
)
{
    std::vector<int> threadCounts =
    {
        1,
        2,
        4,
        8
    };


    std::cout
        << "============================================================\n";

    std::cout
        << "Pipeline Size: "
        << N
        << " symbols\n";

    std::cout
        << "============================================================\n";


    std::cout
        << std::left
        << std::setw(10)
        << "Threads"

        << std::setw(14)
        << "P50(ms)"

        << std::setw(14)
        << "P95(ms)"

        << std::setw(16)
        << "Throughput"

        << std::setw(12)
        << "Speedup"

        << "Correct"

        << "\n";


    std::cout
        << "------------------------------------------------------------"
        << "--------------\n";


    double baselineP50 = 0.0;


    for (int threadCount : threadCounts)
    {
        PipelineResult result =
            runPipelineBenchmark(
                N,
                threadCount,
                warmupIterations,
                measuredIterations
            );


        // 单线程作为 baseline
        if (threadCount == 1)
        {
            baselineP50 =
                result.performance.p50Ms;
        }


        double speedup =
            baselineP50
            /
            result.performance.p50Ms;


        std::cout
            << std::left
            << std::setw(10)
            << threadCount

            << std::setw(14)
            << result.performance.p50Ms

            << std::setw(14)
            << result.performance.p95Ms

            << std::setw(16)
            << result.throughput

            << std::setw(12)
            << speedup

            << (
                result.correctness
                ? "PASS"
                : "FAIL"
            )

            << "\n";
    }


    std::cout << "\n";
}


// ============================================================
// main
// ============================================================

int main()
{
    std::cout
        << "============================================================\n";

    std::cout
        << "End-to-End OFDM Thread Scaling Benchmark\n";

    std::cout
        << "============================================================\n\n";


    // --------------------------------------------------------
    // 100K workload
    // --------------------------------------------------------

    runThreadScaling(
        100000,
        10,
        100
    );


    // --------------------------------------------------------
    // 1M workload
    //
    // 计算量更大，因此减少 Benchmark 次数
    // --------------------------------------------------------

    runThreadScaling(
        1000000,
        5,
        30
    );


    return 0;
}