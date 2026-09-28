#include "ofdm.hpp"

#include <stdexcept>


OfdmEngine::OfdmEngine(int size)
    : size_(size)
{
    // FFTW专用内存
    input_ = fftw_alloc_complex(size_);
    output_ = fftw_alloc_complex(size_);

    if (input_ == nullptr || output_ == nullptr)
    {
        throw std::runtime_error("Failed to allocate FFTW buffers.");
    }


    // 创建IFFT plan
    ifftPlan_ = fftw_plan_dft_1d(
        size_,
        input_,
        output_,
        FFTW_BACKWARD,
        FFTW_ESTIMATE
    );


    // 创建FFT plan
    fftPlan_ = fftw_plan_dft_1d(
        size_,
        input_,
        output_,
        FFTW_FORWARD,
        FFTW_ESTIMATE
    );


    if (ifftPlan_ == nullptr || fftPlan_ == nullptr)
    {
        throw std::runtime_error("Failed to create FFTW plans.");
    }
}



OfdmEngine::~OfdmEngine()
{
    // 释放FFT计划
    fftw_destroy_plan(ifftPlan_);
    fftw_destroy_plan(fftPlan_);

    // 释放FFTW内存
    fftw_free(input_);
    fftw_free(output_);
}



std::vector<std::complex<double>> OfdmEngine::modulate(
    const std::vector<double>& symbols
)
{
    if (static_cast<int>(symbols.size()) != size_)
    {
        throw std::runtime_error(
            "Input size does not match OFDM engine size."
        );
    }


    // 将频域BPSK符号写入FFTW输入缓存
    for (int i = 0; i < size_; ++i)
    {
        input_[i][0] = symbols[i];  // 实部
        input_[i][1] = 0.0;         // 虚部
    }


    // 执行IFFT
    fftw_execute(ifftPlan_);


    std::vector<std::complex<double>> result(size_);


    // FFTW IFFT默认没有除以N，因此手动归一化
    for (int i = 0; i < size_; ++i)
    {
        result[i] = std::complex<double>(
            output_[i][0] / size_,
            output_[i][1] / size_
        );
    }


    return result;
}



std::vector<std::complex<double>> OfdmEngine::demodulate(
    const std::vector<std::complex<double>>& samples
)
{
    if (static_cast<int>(samples.size()) != size_)
    {
        throw std::runtime_error(
            "Input size does not match OFDM engine size."
        );
    }


    // 将时域信号写入FFTW输入缓存
    for (int i = 0; i < size_; ++i)
    {
        input_[i][0] = samples[i].real();
        input_[i][1] = samples[i].imag();
    }


    // 执行FFT
    fftw_execute(fftPlan_);


    std::vector<std::complex<double>> result(size_);


    for (int i = 0; i < size_; ++i)
    {
        result[i] = std::complex<double>(
            output_[i][0],
            output_[i][1]
        );
    }


    return result;
}