#pragma once
#include <expected>
#include <vector>
#include <string>

#include "info/crypto_info.h"
#include "utils/error/app_error.h"

namespace stx
{
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
        std::expected<CryptoInfo,err::Error> encode(const std::string& plaintext);
        std::expected<std::string,err::Error> decode(const CryptoInfo& crypto_info);
    private:
        std::expected<void,err::Error> sodiumKeyGenerateBySalt(const std::vector<unsigned char>& salt);

        // machine binding can terminate: the encryption principle won't work without it.
        static std::string machine_binding();

        std::vector<unsigned char> salt_;
        std::vector<unsigned char> key_;
        std::string machine_id_;
    };
}
