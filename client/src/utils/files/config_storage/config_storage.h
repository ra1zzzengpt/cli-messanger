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

        [[nodiscard]] bool save();

        // - USER -
        void setByLogin(const UserInfo& user, const std::string& password);
        void updatePassword(const std::string& new_password);
        void updateNickname(const std::string& new_nickname);

        // - SERVER -

        void updateHost(const std::string& new_host);
        void updatePort(const std::string& new_port);

        // - CHATS -
        void addChat(const ChatInfo& new_chat);

    private:
        std::string filepath_;
        AppConfig config_;
        CryptoSodium cryptoSodium_;
    };
}
