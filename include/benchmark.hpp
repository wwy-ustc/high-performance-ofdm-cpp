#pragma once

#include <functional>


struct BenchmarkResult
{
   double averageMs;
   double minMs;
   double maxMs;
   double p50Ms;
   double p95Ms;
};


BenchmarkResult benchmark(
    const std::function<void()>& func,
    int warmupIterations = 10,
    int measuredIterations = 100
);