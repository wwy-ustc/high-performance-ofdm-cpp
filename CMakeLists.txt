cmake_minimum_required(VERSION 3.16)

project(ofdm_encryption_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Threads REQUIRED)

# ============================================================
# Main executable
# ============================================================

add_executable(ofdm_encryption_cpp
    src/main.cpp
    src/logistic.cpp
    src/bpsk.cpp
    src/encryption.cpp
    src/ofdm.cpp
    src/benchmark.cpp
)

target_include_directories(
    ofdm_encryption_cpp
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    ofdm_encryption_cpp
    PRIVATE
    fftw3_threads
    fftw3
    Threads::Threads
)


# ============================================================
# End-to-End Pipeline Benchmark
# ============================================================

add_executable(benchmark_pipeline
    benchmarks/benchmark_pipeline.cpp
    src/logistic.cpp
    src/bpsk.cpp
    src/encryption.cpp
    src/ofdm.cpp
    src/benchmark.cpp
)

target_include_directories(
    benchmark_pipeline
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
   benchmark_pipeline
    PRIVATE
    fftw3_threads
    fftw3
    Threads::Threads
)


add_executable(benchmark_fftw
    benchmarks/benchmark_fftw.cpp
    src/ofdm.cpp
)

target_include_directories(
    benchmark_fftw
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    benchmark_fftw
    PRIVATE
    fftw3_threads
    fftw3
    Threads::Threads
)

add_executable(benchmark_fftw_threads
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