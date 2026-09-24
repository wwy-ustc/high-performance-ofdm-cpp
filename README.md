# High Performance OFDM Signal Processing Engine

A modular C++17 implementation of an OFDM signal processing pipeline with FFTW-based numerical computation and performance benchmarking.

This project reconstructs a MATLAB-based communication algorithm prototype into a modern C++ engineering project, focusing on modular design, numerical computing, and performance analysis.

---

## Features

- C++17 modular architecture
- CMake-based build system
- Linux development environment
- BPSK modulation and demodulation
- Logistic chaotic sequence generation
- Chaos-based BPSK encryption
- FFTW-based OFDM FFT/IFFT processing
- Performance benchmark framework

---

## System Pipeline

```
Input Bits

    ↓

BPSK Modulation

    ↓

Chaos-based Encryption

    ↓

OFDM IFFT
(FFTW)

    ↓

Time-domain Signal

    ↓

OFDM FFT
(FFTW)

    ↓

Recovered Frequency Symbols
```

---

## Project Structure

```
high-performance-ofdm-cpp/

├── include/
│   ├── bpsk.hpp
│   ├── logistic.hpp
│   ├── encryption.hpp
│   ├── ofdm.hpp
│   └── benchmark.hpp
│
├── src/
│   ├── main.cpp
│   ├── bpsk.cpp
│   ├── logistic.cpp
│   ├── encryption.cpp
│   ├── ofdm.cpp
│   └── benchmark.cpp
│
├── CMakeLists.txt
└── README.md
```

---

## Requirements

- Linux / WSL Ubuntu
- C++17 compiler
- CMake >= 3.16
- FFTW3

Install FFTW:

```bash
sudo apt update
sudo apt install libfftw3-dev
```

---

## Build

Clone the repository:

```bash
git clone https://github.com/wwy-ustc/high-performance-ofdm-cpp.git

cd high-performance-ofdm-cpp
```

Build:

```bash
mkdir build

cmake -S . -B build

cmake --build build
```

Run:

```bash
./build/ofdm_encryption_cpp
```

---

## Benchmark

The project includes a lightweight benchmark framework to profile different computational modules.

Current test:

- Data size: 100000 symbols


Example result:

| Module | Runtime |
|:---|---:|
| BPSK Modulation | 3.21 ms |
| Logistic Generation | 0.87 ms |
| Encryption | 2.88 ms |
| OFDM IFFT | 4.32 ms |
| OFDM FFT | 3.11 ms |
| Total | 14.39 ms |

Benchmark results may vary depending on CPU hardware.

---

## Technical Highlights

### Modular C++ Design

The project separates different computational operators into independent modules:

- Signal modulation
- Encryption operator
- OFDM transform operator
- Benchmark utilities

This design allows individual components to be optimized and replaced independently.

---

### FFTW Integration

The OFDM transform module uses FFTW3 for high-performance FFT/IFFT computation.

The current implementation supports:

- Frequency-domain to time-domain transformation (IFFT)
- Time-domain to frequency-domain transformation (FFT)

---

### Performance Optimization Roadmap

Future improvements:

- [ ] FFTW multi-thread acceleration
- [ ] OpenMP parallel optimization
- [ ] SIMD/vectorization optimization
- [ ] GPU acceleration with CUDA
- [ ] More comprehensive profiling tools

---

## Motivation

This project aims to explore the engineering transformation of numerical algorithms from research prototypes into efficient C++ computing pipelines.

The project focuses on:

- Modern C++ development
- High-performance numerical computing
- System-level optimization
- Performance benchmarking
