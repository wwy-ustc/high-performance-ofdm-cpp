#include "benchmark.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <vector>


BenchmarkResult benchmark(
    const std::function<void()>& func,
    int warmupIterations,
    int measuredIterations
)
{
    if (measuredIterations <= 0)
    {
        throw std::invalid_argument(
            "measuredIterations must be greater than 0."
        );
    }


    // Warm-up
    for (int i = 0; i < warmupIterations; ++i)
    {
        func();
    }


    std::vector<double> times;
    times.reserve(measuredIterations);


    // Real benchmark
    for (int i = 0; i < measuredIterations; ++i)
    {
        auto start =
            std::chrono::steady_clock::now();

        func();

        auto end =
            std::chrono::steady_clock::now();


        std::chrono::duration<double, std::milli> elapsed =
            end - start;


        times.push_back(elapsed.count());
    }


    double sum = 0.0;

    for (double time : times)
    {
        sum += time;
    }


    double average =
        sum / static_cast<double>(times.size());


    std::sort(times.begin(), times.end());


    auto percentile =
        [&](double p)
        {
            std::size_t index =
                static_cast<std::size_t>(
                    p * (times.size() - 1)
                );

            return times[index];
        };


    BenchmarkResult result;

    result.averageMs = average;

    result.minMs =
        times.front();

    result.p50Ms =
        percentile(0.50);

    result.p95Ms =
        percentile(0.95);

    result.maxMs =
        times.back();


    return result;
}