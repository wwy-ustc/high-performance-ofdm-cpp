#include "ofdm.hpp"

#include <fftw3.h>



std::vector<std::complex<double>> ofdmModulate(
    const std::vector<double>& symbols
)
{

    int N = symbols.size();



    // FFTW输入输出缓存

    std::vector<std::complex<double>> input(N);

    std::vector<std::complex<double>> output(N);



    // =============================
    // 将BPSK符号转换成复数
    //
    // 例如:
    //
    // 1
    // -1
    //
    // 转换:
    //
    // 1+0j
    // -1+0j
    //
    // =============================

    for(int i = 0; i < N; i++)
    {
        input[i] =
            std::complex<double>(
                symbols[i],
                0.0
            );
    }



    // =============================
    // 创建IFFT计划
    //
    // FFTW_BACKWARD = IFFT
    //
    // 频域 → 时域
    //
    // =============================

    fftw_plan plan =
        fftw_plan_dft_1d(

            N,

            reinterpret_cast<fftw_complex*>(
                input.data()
            ),

            reinterpret_cast<fftw_complex*>(
                output.data()
            ),

            FFTW_BACKWARD,

            FFTW_ESTIMATE
        );



    // 执行IFFT

    fftw_execute(plan);



    // FFTW不会自动归一化

    for(auto& value : output)
    {
        value /= N;
    }



    // 释放资源

    fftw_destroy_plan(plan);



    return output;
}





std::vector<std::complex<double>> ofdmDemodulate(
    const std::vector<std::complex<double>>& samples
)
{

    int N = samples.size();



    // 输入输出缓存

    std::vector<std::complex<double>> input(samples);

    std::vector<std::complex<double>> output(N);



    // =============================
    // 创建FFT计划
    //
    // FFTW_FORWARD = FFT
    //
    // 时域 → 频域
    //
    // =============================


    fftw_plan plan =
        fftw_plan_dft_1d(

            N,

            reinterpret_cast<fftw_complex*>(
                input.data()
            ),

            reinterpret_cast<fftw_complex*>(
                output.data()
            ),

            FFTW_FORWARD,

            FFTW_ESTIMATE
        );



    // 执行FFT

    fftw_execute(plan);



    // 释放计划

    fftw_destroy_plan(plan);



    return output;
}