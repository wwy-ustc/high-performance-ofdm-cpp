#include "logistic.hpp"

std::vector<double> generateLogisticSequence(
    int N,
    double x0,
    double r
) 
{
    std::vector<double> sequence(N);
    sequence[0] = x0;
    for (int i = 1; i < N; ++i) {
        sequence[i] = r * sequence[i - 1] * (1 - sequence[i - 1]);
    }
    return sequence;
}