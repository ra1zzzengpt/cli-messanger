#ifndef CRYPTO_INFO_HPP
#define CRYPTO_INFO_HPP

#include <vector>

struct CryptoInfo
{
    std::vector<unsigned char> salt;
    std::vector<unsigned char> nonce;
    std::vector<unsigned char> ciphertext;
};

std::vector<unsigned char> to_export(const CryptoInfo& crypto_info);
CryptoInfo import(const std::vector<unsigned char>& raw_bytes);

#endif