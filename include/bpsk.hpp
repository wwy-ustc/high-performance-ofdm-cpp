#pragma once

#include <vector>

std::vector<double> bpskModulate(
    const std::vector<int>& bits
);

std::vector<int> bpskDemodulate(
    const std::vector<double>& symbols
);
