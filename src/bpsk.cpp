#include"bpsk.hpp"

std::vector<double> bpskModulate(
    const std::vector<int>& bits
) 
{
    std::vector<double> symbols;

    symbols.reserve(bits.size());

    for (int bit : bits) {
        if(bit == 0)
        {
            symbols.push_back(-1.0);
        }
        else
        {
            symbols.push_back(1.0);
        }
    }
    return symbols;
}

std::vector<int> bpskDemodulate(
    const std::vector<double>& symbols
) 
{
    std::vector<int> bits;

    bits.reserve(symbols.size());

    for (double symbol : symbols) {
        if(symbol >= 0.0)
        {
            bits.push_back(1);
        }
        else
        {
            bits.push_back(0);
        }
    }
    return bits;
}