#pragma once

#include <vector>

std::vector<double> encryptBPSK(
    const std::vector<double>& symbols,
    const std::vector<double>& chaos
);

std::vector<double> decryptBPSK(
    const std::vector<double>& encrypted,
    const std::vector<double>& chaos
);