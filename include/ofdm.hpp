#pragma once

#include <complex>
#include <vector>

#include <fftw3.h>


class OfdmEngine
{
public:
    // 构造函数：创建FFT/IFFT执行环境
    explicit OfdmEngine(int size);

    // 析构函数：释放FFTW资源
    ~OfdmEngine();

    // 禁止复制
    OfdmEngine(const OfdmEngine&) = delete;
    OfdmEngine& operator=(const OfdmEngine&) = delete;


    // OFDM调制：频域 -> IFFT -> 时域
    std::vector<std::complex<double>> modulate(
        const std::vector<double>& symbols
    );


    // OFDM解调：时域 -> FFT -> 频域
    std::vector<std::complex<double>> demodulate(
        const std::vector<std::complex<double>>& samples
    );


private:
    int size_;

    fftw_complex* input_;
    fftw_complex* output_;

    fftw_plan ifftPlan_;
    fftw_plan fftPlan_;
};