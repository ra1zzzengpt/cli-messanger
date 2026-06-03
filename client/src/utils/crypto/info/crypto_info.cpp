#include "crypto_info.h"
#include "sodium.h"
#include <span>
#include <stdexcept>

std::vector<unsigned char> to_export(const CryptoInfo &crypto_info)
{
    std::vector<unsigned char> result;
    result.insert(result.begin(),crypto_info.nonce.begin(),crypto_info.nonce.end());
    result.insert(result.begin()+static_cast<long>(crypto_info.nonce.size()),crypto_info.salt.begin(),crypto_info.salt.end());
    result.insert(result.begin()+static_cast<long>(crypto_info.nonce.size()+crypto_info.salt.size()),crypto_info.ciphertext.begin(),crypto_info.ciphertext.end());
    return result;
}

CryptoInfo import(const std::vector<unsigned char>& raw_bytes)
{
    if (raw_bytes.size() < crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES)
    {
        throw std::runtime_error("hasn't needed data");
    }
    CryptoInfo crypto_info;
    crypto_info.nonce.resize(crypto_secretbox_NONCEBYTES);
    for (std::size_t i = 0; i < crypto_secretbox_NONCEBYTES; ++i)
    {
        crypto_info.nonce[i] = raw_bytes[i];
    }
    crypto_info.salt.resize(crypto_pwhash_SALTBYTES);
    for (std::size_t i = crypto_secretbox_NONCEBYTES; i < crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES; ++i)
    {
        crypto_info.salt[i - crypto_secretbox_NONCEBYTES] = raw_bytes[i];
    }
    crypto_info.ciphertext.resize(raw_bytes.size() - crypto_secretbox_NONCEBYTES - crypto_pwhash_SALTBYTES);
    for (std::size_t i = crypto_secretbox_NONCEBYTES + crypto_pwhash_SALTBYTES; i < raw_bytes.size(); ++i)
    {
        crypto_info.ciphertext[i - crypto_secretbox_NONCEBYTES - crypto_pwhash_SALTBYTES] = raw_bytes[i];
    }
    return crypto_info;
}