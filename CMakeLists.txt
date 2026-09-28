cmake_minimum_required(VERSION 3.16)

project(ofdm_encryption_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)


# ============================================================
# Core Library
#
# 所有 OFDM 核心计算模块统一编译成一个静态库
# ============================================================

add_library(ofdm_core STATIC
    src/bpsk.cpp
    src/logistic.cpp
    src/encryption.cpp
    src/ofdm.cpp
)

target_include_directories(
    ofdm_core
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    ofdm_core
    PUBLIC
    fftw3_threads
    fftw3
    Threads::Threads
)


# ============================================================
# Main Program
# ============================================================

add_executable(
    ofdm_encryption_cpp
    src/main.cpp
)

target_link_libraries(
    ofdm_encryption_cpp
    PRIVATE
    ofdm_core
)


# ============================================================
# End-to-End Pipeline Benchmark
# ============================================================

add_executable(
    benchmark_pipeline
    benchmarks/benchmark_pipeline.cpp
    src/benchmark.cpp
)

target_link_libraries(
    benchmark_pipeline
    PRIVATE
    ofdm_core
)


# ============================================================
# FFTW Plan-Reuse / Workload Scaling Benchmark
# ============================================================

add_executable(
    benchmark_fftw
    benchmarks/benchmark_fftw.cpp
)

target_link_libraries(
    benchmark_fftw
    PRIVATE
    ofdm_core
)


# ============================================================
# FFTW Multi-thread Microbenchmark
# ============================================================

add_executable(
    benchmark_fftw_threads
    benchmarks/benchmark_fftw_threads.cpp
)

target_include_directories(
    benchmark_fftw_threads
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    benchmark_fftw_threads
    PRIVATE
    fftw3_threads
    fftw3
    Threads::Threads
)