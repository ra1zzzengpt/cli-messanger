#include "crypto_info.h"
#include "base64.h"

namespace
{
    std::vector<unsigned char> read_base64_field(const nlohmann::json& json, const std::string& key)
    {
        if (const std::expected<std::vector<unsigned char>,stx::err::AppError> result = stx::crypto::from_base64(json[key]); result.has_value())
        {
            return std::vector{*result};
        }
        return std::vector<unsigned char>{};
    }
}

void to_json(nlohmann::json &json, const CryptoInfo &crypto_info)
{
    json = nlohmann::json {
        {"salt", stx::crypto::to_base64(crypto_info.salt)},
        {"nonce", stx::crypto::to_base64(crypto_info.nonce)},
        {"ciphertext", stx::crypto::to_base64(crypto_info.ciphertext)}
    };
}

void from_json(const nlohmann::json& json, CryptoInfo& crypto_info)
{
    crypto_info.salt = read_base64_field(json, "salt");
    crypto_info.nonce = read_base64_field(json, "nonce");
    crypto_info.ciphertext = read_base64_field(json, "ciphertext");
}