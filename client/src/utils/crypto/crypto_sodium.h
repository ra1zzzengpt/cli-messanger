#pragma once
#include <vector>
#include <string>
#include <optional>

#include "helpers/crypto_info.h"

class CryptoSodium
{
public:
    // - OBJ -
    CryptoSodium();
    ~CryptoSodium();
    CryptoSodium(const CryptoSodium&) = delete;
    CryptoSodium& operator=(const CryptoSodium&) = delete;
    CryptoSodium(CryptoSodium&&) = delete;
    CryptoSodium& operator=(CryptoSodium&&) = delete;

    // - CODEC -
    CryptoInfo encode(const std::string& plaintext);
    std::optional<std::string> decode(const CryptoInfo& crypto_info); // todo: to expected
private:
    void sodiumKeyGenerateBySalt(const std::vector<unsigned char>& salt); // todo: to expected

    static std::string machine_binding(); // todo: to expected

    std::vector<unsigned char> salt_;
    std::vector<unsigned char> key_;
    std::string machine_id_;
};
