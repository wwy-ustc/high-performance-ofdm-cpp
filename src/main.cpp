#include <iostream>
#include <vector>
#include <complex>
#include <random>

#include "bpsk.hpp"
#include "logistic.hpp"
#include "encryption.hpp"
#include "ofdm.hpp"


int main()
{
    // ========================================
    // 1. 基本配置
    // ========================================

    const int N = 100000;


    // ========================================
    // 2. 生成固定输入 bits
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
    // 3. BPSK 调制
    //
    // 0 -> -1
    // 1 -> +1
    // ========================================

    std::vector<double> symbols =
        bpskModulate(bits);


    // ========================================
    // 4. 生成 Logistic 混沌序列
    // ========================================

    std::vector<double> chaos =
        generateLogisticSequence(
            N,
            0.54321,
            3.999
        );


    // ========================================
    // 5. 加密
    // ========================================

    std::vector<double> encrypted =
        encryptBPSK(
            symbols,
            chaos
        );


    // ========================================
    // 6. 创建 OFDM Engine
    //
    // 在构造函数中：
    // - 分配 FFTW Buffer
    // - 创建 FFT Plan
    // - 创建 IFFT Plan
    //
    // 后续重复复用这些资源
    // ========================================

    OfdmEngine ofdm(N);


    // ========================================
    // 7. OFDM 调制
    //
    // Frequency Domain
    //      ↓
    //     IFFT
    //      ↓
    // Time Domain
    // ========================================

    std::vector<std::complex<double>>
        timeSamples =
            ofdm.modulate(
                encrypted
            );


    // ========================================
    // 8. OFDM 解调
    //
    // Time Domain
    //      ↓
    //     FFT
    //      ↓
    // Frequency Domain
    // ========================================

    std::vector<std::complex<double>>
        recovered =
            ofdm.demodulate(
                timeSamples
            );


    // ========================================
    // 9. FFT 数值正确性检查
    //
    // 理论上：
    //
    // encrypted
    //   ↓
    // IFFT
    //   ↓
    // FFT
    //   ↓
    // recovered
    //
    // recovered 应该与 encrypted 基本一致
    // ========================================

    double maxError = 0.0;


    for (std::size_t i = 0;
         i < encrypted.size();
         ++i)
    {
        std::complex<double> expected(
            encrypted[i],
            0.0
        );


        double error =
            std::abs(
                recovered[i]
                -
                expected
            );


        if (error > maxError)
        {
            maxError = error;
        }
    }


    // ========================================
    // 10. complex<double> -> double
    //
    // FFT 输出是复数，
    // 但 BPSK / Encryption 使用 double。
    //
    // 因此取实部作为恢复后的加密符号。
    // ========================================

    std::vector<double>
        recoveredEncrypted(N);


    for (int i = 0; i < N; ++i)
    {
        recoveredEncrypted[i] =
            recovered[i].real();
    }


    // ========================================
    // 11. 解密
    // ========================================

    std::vector<double> decrypted =
        decryptBPSK(
            recoveredEncrypted,
            chaos
        );


    // ========================================
    // 12. BPSK 解调
    //
    // -1 -> 0
    // +1 -> 1
    // ========================================

    std::vector<int> recoveredBits =
        bpskDemodulate(
            decrypted
        );


    // ========================================
    // 13. 输出正确性结果
    // ========================================

    std::cout
        << "====================================\n";

    std::cout
        << "High Performance OFDM Engine\n";

    std::cout
        << "====================================\n";

    std::cout
        << "Data size: "
        << N
        << " symbols\n\n";


    std::cout
        << "Maximum FFT reconstruction error: "
        << maxError
        << "\n";


    if (maxError < 1e-9)
    {
        std::cout
            << "FFT Reconstruction: PASS\n";
    }
    else
    {
        std::cout
            << "FFT Reconstruction: FAIL\n";
    }


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