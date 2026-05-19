#pragma once
#include <expected>

#include "models/app_config.h"
#include "utils/crypto/crypto_sodium.h"
#include "utils/error/app_error.h"

namespace utils
{
    class ConfigStorage final
    {
    public:
        explicit ConfigStorage(std::string filepath);
        ~ConfigStorage() = default;

        [[nodiscard]] const AppConfig& getConfig() const noexcept;

        std::expected<void,errors::AppError> load();

        std::expected<void,errors::AppError> save();

        // - USER -
        std::expected<void,errors::AppError> setByLogin(const UserInfo& user, const std::string& password);
        std::expected<void,errors::AppError> updatePassword(const std::string& new_password);
        std::expected<void,errors::AppError> updateNickname(const std::string& new_nickname);

        // - SERVER -
        std::expected<void,errors::AppError> updateUrl(const std::string& new_url);

        // - CHATS -
        std::expected<void,errors::AppError> addChat(const ChatInfo& new_chat);

    private:
        std::string filepath_;
        AppConfig config_;
        CryptoSodium cryptoSodium_;
    };
}
