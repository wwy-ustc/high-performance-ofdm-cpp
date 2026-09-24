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

    // ===================================
    // 1. 设置测试数据规模
    //
    // AI Infra benchmark通常使用较大规模数据
    //
    // ===================================

    const int N = 100000;



    // ===================================
    // 2. 生成随机bit数据
    //
    // 模拟真实输入
    //
    // ===================================

    std::vector<int> bits(N);


    std::random_device rd;

    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(0,1);



    for(int i = 0; i < N; i++)
    {
        bits[i] = dist(gen);
    }



    std::vector<double> symbols;

    std::vector<double> chaos;

    std::vector<double> encrypted;


    std::vector<std::complex<double>> timeSamples;

    std::vector<std::complex<double>> recovered;




    // ===================================
    // Benchmark 1:
    // BPSK modulation
    // ===================================


    double bpskTime =
        measureTime(
            [&]()
            {
                symbols =
                    bpskModulate(bits);
            }
        );



    // ===================================
    // Benchmark 2:
    // Logistic sequence generation
    // ===================================


    double logisticTime =
        measureTime(
            [&]()
            {
                chaos =
                    generateLogisticSequence(
                        N,
                        0.54321,
                        3.999
                    );
            }
        );




    // ===================================
    // Benchmark 3:
    // Encryption
    // ===================================


    double encryptionTime =
        measureTime(
            [&]()
            {
                encrypted =
                    encryptBPSK(
                        symbols,
                        chaos
                    );
            }
        );





    // ===================================
    // Benchmark 4:
    // OFDM IFFT
    // ===================================


    double ifftTime =
        measureTime(
            [&]()
            {
                timeSamples =
                    ofdmModulate(
                        encrypted
                    );
            }
        );





    // ===================================
    // Benchmark 5:
    // OFDM FFT
    // ===================================


    double fftTime =
        measureTime(
            [&]()
            {
                recovered =
                    ofdmDemodulate(
                        timeSamples
                    );
            }
        );





    // ===================================
    // 输出benchmark结果
    // ===================================


    std::cout
        << "============================"
        << std::endl;


    std::cout
        << "Performance Benchmark"
        << std::endl;


    std::cout
        << "Data size: "
        << N
        << " symbols"
        << std::endl;


    std::cout
        << "============================"
        << std::endl;



    std::cout
        << "BPSK modulation: "
        << bpskTime
        << " ms"
        << std::endl;



    std::cout
        << "Logistic generation: "
        << logisticTime
        << " ms"
        << std::endl;



    std::cout
        << "Encryption: "
        << encryptionTime
        << " ms"
        << std::endl;



    std::cout
        << "IFFT: "
        << ifftTime
        << " ms"
        << std::endl;



    std::cout
        << "FFT: "
        << fftTime
        << " ms"
        << std::endl;



    double total =
        bpskTime
        + logisticTime
        + encryptionTime
        + ifftTime
        + fftTime;



    std::cout
        << "----------------------------"
        << std::endl;


    std::cout
        << "Total:"
        << total
        << " ms"
        << std::endl;


    std::cout
        << "============================"
        << std::endl;



    return 0;
}