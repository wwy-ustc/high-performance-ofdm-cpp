#include "benchmark.hpp"

#include <chrono>


double measureTime(
    std::function<void()> func
)
{

    auto start =
        std::chrono::high_resolution_clock::now();



    func();



    auto end =
        std::chrono::high_resolution_clock::now();



    std::chrono::duration<double, std::milli>
    elapsed =
        end - start;



    return elapsed.count();
}