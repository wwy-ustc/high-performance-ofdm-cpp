#include <iostream>
#include <vector>
#include <complex>
#include <random>

#include "bpsk.hpp"
#include "logistic.hpp"
#include "encryption.hpp"
#include "ofdm.hpp"
#include "benchmark.hpp"


int main()
{
    // ========================================
    // Benchmark 配置
    // ========================================

    const int N = 100000;

    const int warmupIterations = 10;
    const int measuredIterations = 100;


    // ========================================
    // 固定输入
    // ========================================

    std::vector<int> bits(N);

    std::mt19937 generator(42);

    std::uniform_int_distribution<int>
        distribution(0, 1);


    for (int i = 0; i < N; ++i)
    {
        bits[i] =
            distribution(generator);
    }


    // ========================================
    // Pipeline 中间结果
    // ========================================

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


    // ========================================
    // OFDM Engine
    //
    // FFTW Plan / Buffer 初始化一次，
    // Benchmark 测运行阶段延迟。
    // ========================================

    OfdmEngine ofdm(N);


    // ========================================
    // End-to-End Benchmark
    // ========================================

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


                // 6. complex -> real
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


    // ========================================
    // Throughput
    //
    // P50 是毫秒，因此先除以1000变成秒
    // ========================================

    double throughput =
        static_cast<double>(N)
        /
        (result.p50Ms / 1000.0);


    // ========================================
    // 输出
    // ========================================

    std::cout
        << "====================================\n";

    std::cout
        << "End-to-End OFDM Pipeline Benchmark\n";

    std::cout
        << "====================================\n";

    std::cout
        << "Data size: "
        << N
        << " symbols\n";

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


    std::cout
        << "Average: "
        << result.averageMs
        << " ms\n";

    std::cout
        << "Min: "
        << result.minMs
        << " ms\n";

    std::cout
        << "P50: "
        << result.p50Ms
        << " ms\n";

    std::cout
        << "P95: "
        << result.p95Ms
        << " ms\n";

    std::cout
        << "Max: "
        << result.maxMs
        << " ms\n";


    std::cout
        << "\nP50 Throughput: "
        << throughput
        << " symbols/s\n";


    // ========================================
    // End-to-End Correctness
    // ========================================

    if (bits == recoveredBits)
    {
        std::cout
            << "End-to-End Bit Recovery: PASS\n";
    }
    else
    {
        std::cout
            << "End-to-End Bit Recovery: FAIL\n";
    }


    return 0;
}