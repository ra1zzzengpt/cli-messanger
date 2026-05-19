#pragma once
#include "models/app_config.h"
#include "utils/crypto/crypto_sodium.h"

namespace utils
{
    class ConfigStorage final
    {
    public:
        explicit ConfigStorage(std::string filepath);
        ~ConfigStorage() = default;

        [[nodiscard]] const AppConfig& getConfig() const noexcept;

        void load();

        [[nodiscard]] bool save(); // todo: to expected

        // - USER -
        void setByLogin(const UserInfo& user, const std::string& password); // todo: to expected
        void updatePassword(const std::string& new_password); // todo: to expected
        void updateNickname(const std::string& new_nickname); // todo: to expected

        // - SERVER -
        void updateUrl(const std::string& new_url); // todo: to expected

        // - CHATS -
        void addChat(const ChatInfo& new_chat); // todo: to expected

    private:
        std::string filepath_;
        AppConfig config_;
        CryptoSodium cryptoSodium_;
    };
}
