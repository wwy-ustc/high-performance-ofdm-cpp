#pragma once

#include <vector>
#include <complex>


// =============================
// OFDM调制
//
// 输入:
// 频域BPSK符号
//
// 输出:
// IFFT后的时域复数采样
// =============================

std::vector<std::complex<double>> ofdmModulate(
    const std::vector<double>& symbols
);



// =============================
// OFDM解调
//
// 输入:
// 时域复数采样
//
// 输出:
// FFT后的频域符号
// =============================

std::vector<std::complex<double>> ofdmDemodulate(
    const std::vector<std::complex<double>>& samples
);