#include "encryption.hpp"
#include "cstddef"
#include <omp.h>

std::vector<double> encryptBPSK(
    const std::vector<double>& symbols,
    const std::vector<double>& chaos
) 
{
    std::vector<double> encrypted(symbols.size());

    #pragma omp parallel for
    for (std::size_t i = 0; i < symbols.size(); ++i) {
        int symbolBit;

        if(symbols[i] >= 0.0)
        {
            symbolBit = 1;
        }
        else
        {
            symbolBit = 0;
        }

        int chaosBit;
        
        if(chaos[i] >= 0.5)
        {
            chaosBit = 1;
        }
        else
        {
            chaosBit = 0;
        }

        int encryptedBit = symbolBit ^ chaosBit;

        double encryptedSymbol;

        if(encryptedBit == 1)
        {
            encryptedSymbol = 1.0;
        }
        else
        {
            encryptedSymbol = -1.0;
        }


        encrypted[i] = encryptedSymbol;
    }

    return encrypted;
}

// 解密函数
std::vector<double> decryptBPSK(
    const std::vector<double>& encrypted,
    const std::vector<double>& chaos
)
{
    std::vector<double> decrypted;

    decrypted.reserve(encrypted.size());

    for (std::size_t i = 0; i < encrypted.size(); ++i)
    {
        // 加密后的BPSK转换成bit
        int encryptedBit;

        if (encrypted[i] > 0)
        {
            encryptedBit = 1;
        }
        else
        {
            encryptedBit = 0;
        }


        // 混沌序列二值化
        int chaosBit;

        if (chaos[i] > 0.5)
        {
            chaosBit = 1;
        }
        else
        {
            chaosBit = 0;
        }


        // XOR恢复原bit
        int decryptedBit = encryptedBit ^ chaosBit;


        // bit重新映射回BPSK
        double symbol;

        if (decryptedBit == 1)
        {
            symbol = 1.0;
        }
        else
        {
            symbol = -1.0;
        }


        decrypted.push_back(symbol);
    }


    return decrypted;
}