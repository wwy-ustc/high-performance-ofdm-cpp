#pragma once

#include <complex>
#include <vector>

#include <fftw3.h>


class OfdmEngine
{
public:
    // size: FFT 点数
    // threadCount: FFTW 使用的线程数，默认单线程
    explicit OfdmEngine(
        int size,
        int threadCount = 1
    );

    ~OfdmEngine();


    // 禁止复制
    OfdmEngine(
        const OfdmEngine&
    ) = delete;

    OfdmEngine& operator=(
        const OfdmEngine&
    ) = delete;


    // 频域 -> IFFT -> 时域
    std::vector<std::complex<double>>
    modulate(
        const std::vector<double>& symbols
    );


    // 时域 -> FFT -> 频域
    std::vector<std::complex<double>>
    demodulate(
        const std::vector<std::complex<double>>& samples
    );


private:
    int size_;

    int threadCount_;

    fftw_complex* input_;

    fftw_complex* output_;

    fftw_plan ifftPlan_;

    fftw_plan fftPlan_;
};