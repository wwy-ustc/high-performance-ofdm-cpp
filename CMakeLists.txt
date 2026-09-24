cmake_minimum_required(VERSION 3.16)

project(ofdm_encryption_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

add_executable(ofdm_encryption_cpp
    src/main.cpp
    src/logistic.cpp
    src/bpsk.cpp
    src/encryption.cpp
    src/ofdm.cpp
    src/benchmark.cpp
)

target_include_directories(ofdm_encryption_cpp PRIVATE include)

target_link_libraries(
    ofdm_encryption_cpp
    fftw3
    OpenMP::OpenMP_CXX
)
