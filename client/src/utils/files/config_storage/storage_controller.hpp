#pragma once
#include <expected>

#include <utils/files/config_storage/i_storage_controller.hpp>
#include <models/app_config.hpp>
#include <utils/crypto/crypto_sodium.hpp>
#include <utils/error/app_error.hpp>

namespace stx
{
    class StorageController final : public IStorageController
    {
    public:
        explicit StorageController(std::string filepath);
        ~StorageController() override = default;

        [[nodiscard]] const AppConfig& getConfig() const noexcept override;

        std::expected<void,err::Error> load() override;

        std::expected<void,err::Error> save() override;

        void clear() override;

        // - USER -
        std::expected<void,err::Error> updateUser(const UserInfo& user) override;
        std::expected<void,err::Error> updatePassword(const std::string& new_password) override;
        std::expected<void,err::Error> updateNickname(const std::string& new_nickname) override;
        std::expected<void,err::Error> updateID(const uint64_t& new_id) override;

        // - SERVER -
        std::expected<void,err::Error> updateUrl(const std::string& new_url) override;

        // - CHATS -
        std::expected<void,err::Error> addChat(const ChatInfo& new_chat) override;

    private:
        std::string filepath_;
        AppConfig config_;
        CryptoSodium cryptoSodium_;
    };
}
