#pragma once

#include <nlohmann/json.hpp>
#include <vector>

struct CryptoInfo
{
    std::vector<unsigned char> salt;
    std::vector<unsigned char> nonce;
    std::vector<unsigned char> ciphertext;
};

void to_json(nlohmann::json& json, const CryptoInfo& crypto_info);
void from_json(const nlohmann::json& json, CryptoInfo& crypto_info);
