#pragma once
#include <expected>

#include "models/app_config.hpp"
#include "utils/crypto/crypto_sodium.hpp"
#include "utils/error/app_error.hpp"

namespace stx
{
    class ConfigStorage final
    {
    public:
        explicit ConfigStorage(std::string filepath);
        ~ConfigStorage() = default;

        [[nodiscard]] const AppConfig& getConfig() const noexcept;

        std::expected<void,err::Error> load();

        std::expected<void,err::Error> save();

        // - USER -
        std::expected<void,err::Error> setByLogin(const UserInfo& user, const std::string& password);
        std::expected<void,err::Error> updatePassword(const std::string& new_password);
        std::expected<void,err::Error> updateNickname(const std::string& new_nickname);

        std::expected<void,err::Error> setInitialUser(uint64_t id, const std::string& nickname);

        // - SERVER -
        std::expected<void,err::Error> updateUrl(const std::string& new_url);

        // - CHATS -
        std::expected<void,err::Error> addChat(const ChatInfo& new_chat);

    private:
        std::string filepath_;
        AppConfig config_;
        CryptoSodium cryptoSodium_;
    };
}
