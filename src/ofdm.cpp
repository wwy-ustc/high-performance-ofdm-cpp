#include "ofdm.hpp"

#include <mutex>
#include <stdexcept>


// ============================================================
// FFTW 多线程只需要初始化一次
// ============================================================

namespace
{
    std::once_flag fftwThreadsInitFlag;


    void initializeFFTWThreads()
    {
        if (fftw_init_threads() == 0)
        {
            throw std::runtime_error(
                "Failed to initialize FFTW threads."
            );
        }
    }
}


// ============================================================
// Constructor
// ============================================================

OfdmEngine::OfdmEngine(
    int size,
    int threadCount
)
    : size_(size),
      threadCount_(threadCount)
{
    if (size_ <= 0)
    {
        throw std::invalid_argument(
            "OFDM size must be greater than 0."
        );
    }


    if (threadCount_ <= 0)
    {
        throw std::invalid_argument(
            "Thread count must be greater than 0."
        );
    }


    // FFTW 线程模块全局初始化一次
    std::call_once(
        fftwThreadsInitFlag,
        initializeFFTWThreads
    );


    // FFTW Buffer
    input_ =
        fftw_alloc_complex(size_);

    output_ =
        fftw_alloc_complex(size_);


    if (input_ == nullptr ||
        output_ == nullptr)
    {
        if (input_ != nullptr)
        {
            fftw_free(input_);
        }

        if (output_ != nullptr)
        {
            fftw_free(output_);
        }

        throw std::runtime_error(
            "Failed to allocate FFTW buffers."
        );
    }


    // ========================================================
    // 创建 IFFT Plan
    // ========================================================

    fftw_plan_with_nthreads(
        threadCount_
    );


    ifftPlan_ =
        fftw_plan_dft_1d(
            size_,
            input_,
            output_,
            FFTW_BACKWARD,
            FFTW_ESTIMATE
        );


    // ========================================================
    // 创建 FFT Plan
    // ========================================================

    fftw_plan_with_nthreads(
        threadCount_
    );


    fftPlan_ =
        fftw_plan_dft_1d(
            size_,
            input_,
            output_,
            FFTW_FORWARD,
            FFTW_ESTIMATE
        );


    if (ifftPlan_ == nullptr ||
        fftPlan_ == nullptr)
    {
        if (ifftPlan_ != nullptr)
        {
            fftw_destroy_plan(
                ifftPlan_
            );
        }


        if (fftPlan_ != nullptr)
        {
            fftw_destroy_plan(
                fftPlan_
            );
        }


        fftw_free(input_);

        fftw_free(output_);


        throw std::runtime_error(
            "Failed to create FFTW plans."
        );
    }
}


// ============================================================
// Destructor
// ============================================================

OfdmEngine::~OfdmEngine()
{
    fftw_destroy_plan(
        ifftPlan_
    );

    fftw_destroy_plan(
        fftPlan_
    );


    fftw_free(
        input_
    );

    fftw_free(
        output_
    );
}


// ============================================================
// OFDM Modulation
//
// Frequency Domain
//      ↓
//     IFFT
//      ↓
// Time Domain
// ============================================================

std::vector<std::complex<double>>
OfdmEngine::modulate(
    const std::vector<double>& symbols
)
{
    if (
        static_cast<int>(
            symbols.size()
        ) != size_
    )
    {
        throw std::runtime_error(
            "Input size does not match OFDM engine size."
        );
    }


    // 输入写入 FFTW Buffer
    for (int i = 0; i < size_; ++i)
    {
        input_[i][0] =
            symbols[i];

        input_[i][1] =
            0.0;
    }


    // 多线程 IFFT
    fftw_execute(
        ifftPlan_
    );


    std::vector<std::complex<double>>
        result(size_);


    // FFTW BACKWARD 默认没有 /N
    for (int i = 0; i < size_; ++i)
    {
        result[i] =
            std::complex<double>(
                output_[i][0]
                    /
                    static_cast<double>(
                        size_
                    ),

                output_[i][1]
                    /
                    static_cast<double>(
                        size_
                    )
            );
    }


    return result;
}


// ============================================================
// OFDM Demodulation
//
// Time Domain
//      ↓
//     FFT
//      ↓
// Frequency Domain
// ============================================================

std::vector<std::complex<double>>
OfdmEngine::demodulate(
    const std::vector<std::complex<double>>& samples
)
{
    if (
        static_cast<int>(
            samples.size()
        ) != size_
    )
    {
        throw std::runtime_error(
            "Input size does not match OFDM engine size."
        );
    }


    // 输入写入 FFTW Buffer
    for (int i = 0; i < size_; ++i)
    {
        input_[i][0] =
            samples[i].real();

        input_[i][1] =
            samples[i].imag();
    }


    // 多线程 FFT
    fftw_execute(
        fftPlan_
    );


    std::vector<std::complex<double>>
        result(size_);


    for (int i = 0; i < size_; ++i)
    {
        result[i] =
            std::complex<double>(
                output_[i][0],
                output_[i][1]
            );
    }


    return result;
}