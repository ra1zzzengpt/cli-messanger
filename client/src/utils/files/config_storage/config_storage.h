#pragma once
#include <expected>

#include "models/app_config.h"
#include "utils/crypto/crypto_sodium.h"
#include "utils/error/app_error.h"

namespace stx
{
    class ConfigStorage final
    {
    public:
        explicit ConfigStorage(std::string filepath);
        ~ConfigStorage() = default;

        [[nodiscard]] const AppConfig& getConfig() const noexcept;

        std::expected<void,err::AppError> load();

        std::expected<void,err::AppError> save();

        // - USER -
        std::expected<void,err::AppError> setByLogin(const UserInfo& user, const std::string& password);
        std::expected<void,err::AppError> updatePassword(const std::string& new_password);
        std::expected<void,err::AppError> updateNickname(const std::string& new_nickname);

        std::expected<void,err::AppError> setInitialUser(uint64_t id, const std::string& nickname);

        // - SERVER -
        std::expected<void,err::AppError> updateUrl(const std::string& new_url);

        // - CHATS -
        std::expected<void,err::AppError> addChat(const ChatInfo& new_chat);

    private:
        std::string filepath_;
        AppConfig config_;
        CryptoSodium cryptoSodium_;
    };
}
