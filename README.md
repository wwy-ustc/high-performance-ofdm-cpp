# High Performance OFDM C++ Engine

A high-performance OFDM signal processing pipeline implemented in modern C++17, focusing on **FFT acceleration, memory reuse, benchmarking methodology, and parallel execution analysis**.

This project is designed as a performance-oriented C++ system with emphasis on **HPC-style optimization techniques**, including FFTW plan reuse, workload scaling analysis, and multi-thread execution benchmarking.

---

# Overview

This project implements an end-to-end OFDM processing pipeline:

```
Input Bits
    |
    v
BPSK Modulation
    |
    v
Chaos-based Encryption
    |
    v
OFDM Modulation
    |
    v
FFTW IFFT Engine
    |
    v
FFT Demodulation
    |
    v
Decryption
    |
    v
Bit Recovery
```

The system supports:

- Complete OFDM modulation and demodulation
- FFT/IFFT acceleration using FFTW3
- Reusable FFT execution plans
- Configurable multi-thread FFT execution
- End-to-end performance benchmarking
- Correctness verification

---

# Project Structure

```
high-performance-ofdm-cpp/

├── include/
│   ├── bpsk.hpp
│   ├── encryption.hpp
│   ├── logistic.hpp
│   ├── ofdm.hpp
│   └── benchmark.hpp
│
├── src/
│   ├── bpsk.cpp
│   ├── encryption.cpp
│   ├── logistic.cpp
│   ├── ofdm.cpp
│   ├── benchmark.cpp
│   └── main.cpp
│
├── benchmarks/
│   ├── benchmark_pipeline.cpp
│   ├── benchmark_fftw.cpp
│   └── benchmark_fftw_threads.cpp
│
└── CMakeLists.txt
```

---

# Features

## Modern C++ Implementation

- C++17
- Modular CMake build system
- Static library architecture
- RAII-based FFT resource management

Core modules:

- BPSK modulation/demodulation
- Chaos sequence generation
- Encryption/decryption pipeline
- OFDM modulation and demodulation
- FFTW optimized FFT engine

---

# FFTW Optimization

## FFTW Plan Reuse

Repeated FFT plan creation introduces unnecessary initialization overhead.

The project implements reusable FFTW execution plans and persistent buffers.

Legacy workflow:

```
Create FFT Plan
        |
        v
Execute FFT
        |
        v
Destroy Plan
```

Optimized workflow:

```
Create FFT Plan
        |
        v
Execute FFT
        |
        v
Reuse Plan
        |
        v
Execute FFT
```

## Workload Scaling Benchmark

Power-of-two FFT sizes:

| FFT Size | Legacy P50(ms) | Optimized P50(ms) | Speedup |
|---|---:|---:|---:|
|1024|0.0113|0.0048|2.33x|
|16384|0.1697|0.1481|1.15x|
|131072|1.6032|1.5433|1.04x|
|1048576|37.9567|33.5433|1.13x|

Observation:

- Small FFT workloads benefit significantly from eliminating plan creation overhead.
- Large FFT workloads are increasingly dominated by FFT computation cost.

---

# FFTW Multi-thread Scaling

The FFT engine supports configurable FFTW multi-thread execution.

Benchmark workload:

```
IFFT Size: 1048576
```

| Threads | P50 Latency(ms) | Speedup |
|---|---:|---:|
|1|31.02|1.00x|
|2|20.37|1.52x|
|4|16.71|1.86x|
|8|13.84|2.24x|

Results show:

- Parallel execution improves large FFT workload performance.
- Thread scaling is workload dependent.
- Increasing thread count does not always guarantee better performance.

---

# End-to-End Pipeline Benchmark

The complete OFDM pipeline includes:

```
BPSK
 ↓
Logistic Sequence Generation
 ↓
Encryption
 ↓
IFFT
 ↓
FFT
 ↓
Decryption
 ↓
BPSK Demodulation
```

Benchmark metrics:

- P50 latency
- P95 latency
- Throughput
- Correctness verification

---

## 100K Symbols

| Threads | P50(ms) | Speedup |
|---|---:|---:|
|1|5.98|1.00x|
|2|6.04|0.99x|
|4|5.21|1.15x|
|8|6.07|0.99x|

Observation:

For smaller workloads, multi-thread overhead can offset computation benefits.

---

## 1M Symbols

| Threads | P50(ms) | Speedup |
|---|---:|---:|
|1|84.23|1.00x|
|2|70.80|1.19x|
|4|70.79|1.19x|
|8|67.95|1.24x|

Observation:

Larger workloads achieve better parallel scaling because FFT computation becomes a larger portion of total execution time.

Correctness verification:

```
End-to-End Bit Recovery: PASS
```

---

# Performance Analysis

This project evaluates optimization from both kernel-level and system-level perspectives.

## Kernel-level optimization

Examples:

- FFTW plan reuse
- FFTW multi-thread execution

## System-level optimization

The complete pipeline is benchmarked to evaluate real application performance.

According to Amdahl's Law:

> Overall acceleration is limited by the fraction of execution that can be parallelized.

Therefore:

- FFT kernel speedup does not directly translate into identical end-to-end speedup.
- Workload size strongly affects the effectiveness of parallel optimization.

---

# Benchmark Framework

All benchmarks use:

- Release build
- Warm-up iterations
- Multiple measured iterations
- P50/P95 latency statistics
- Throughput measurement
- Correctness validation

Example:

```
Warm-up iterations: 10
Measured iterations: 100
```

---

# Build

## Requirements

- Linux / WSL
- C++17 compiler
- CMake >= 3.16
- FFTW3


Install FFTW:

```bash
sudo apt install libfftw3-dev
```

---

## Compile

```bash
cmake -S . -B build-release \
      -DCMAKE_BUILD_TYPE=Release

cmake --build build-release -j
```

---

# Run

## Main Program

```bash
./build-release/ofdm_encryption_cpp
```

Expected output:

```
FFT Reconstruction: PASS
End-to-End Bit Recovery: PASS
```

---

## Benchmarks

### End-to-End Pipeline

```bash
./build-release/benchmark_pipeline
```

### FFTW Plan Reuse Benchmark

```bash
./build-release/benchmark_fftw
```

### FFTW Multi-thread Benchmark

```bash
./build-release/benchmark_fftw_threads
```

---

# Engineering Highlights

- Designed a modular C++ OFDM processing framework
- Built FFTW-based high-performance FFT/IFFT engine
- Applied RAII for FFT resource lifecycle management
- Implemented reproducible benchmarking framework
- Performed workload scaling analysis
- Evaluated multi-thread parallel performance
- Compared kernel-level and end-to-end optimization effects

---

# Future Improvements

Possible extensions:

- SIMD optimization with AVX instructions
- CUDA GPU acceleration
- Memory bandwidth optimization
- FPGA hardware acceleration
- Distributed pipeline execution
