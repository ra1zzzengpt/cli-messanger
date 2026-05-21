#pragma once
#include <expected>
#include "api/message_api/imessage_api.h"
#include "models/app_config.h"
#include "utils/files/config_storage/config_storage.h"

namespace app
{
    class AppController final
    {
    public:
        // - OBJECT -
        AppController(std::unique_ptr<api::IMessageApi> api,
            std::unique_ptr<stx::ConfigStorage> storage);
        ~AppController() = default;
        AppController(AppController&&) = delete;
        AppController& operator=(const AppController&) = delete;
        AppController& operator=(AppController&&) = delete;

        // - CONFIG -
        [[nodiscard]] const AppConfig& getAppConfig() const noexcept;
        [[nodiscard]] const std::vector<ChatInfo>& getChats() const;
        std::expected<void,stx::err::AppError> loadAppConfig();
        std::expected<void,stx::err::AppError> saveAppConfig();
        std::expected<void,stx::err::AppError> setLogin(const UserInfo& user, const std::string& password);
        std::expected<void,stx::err::AppError> setupInitialUser(std::uint64_t id, const std::string& nickname);
        std::expected<void,stx::err::AppError> updateConfigPassword(const std::string& new_password);
        std::expected<void,stx::err::AppError> updateConfigNickname(const std::string& new_nickname);
        std::expected<void,stx::err::AppError> updateConfigUrl(const std::string& new_url);
        std::expected<void,stx::err::AppError> addChat(const ChatInfo& new_chat);

        // - NETWORK -
        std::expected<void,stx::err::AppError> updateUrl(const std::string& new_url);
        [[nodiscard]] std::expected<std::string,stx::err::AppError> ping() const;
        [[nodiscard]] std::expected<std::vector<Message>,stx::err::AppError> getMessages(const UserInfo& other_user) const;
        [[nodiscard]] std::expected<void,stx::err::AppError> sendMessage(const UserInfo& other_user, const std::string& text) const;
        [[nodiscard]] std::expected<void,stx::err::AppError> updatePassword(const std::string& new_password) const;
        [[nodiscard]] std::expected<void,stx::err::AppError> updateNickname(const std::string& new_nickname) const;
        [[nodiscard]] std::expected<UserInfo,stx::err::AppError> getNicknameById(std::uint64_t id) const;
        [[nodiscard]] std::expected<void,stx::err::AppError> registerUser(const UserInfo& user) const;
        [[nodiscard]] std::expected<void,stx::err::AppError> loginUser(std::uint64_t id, const std::string& password) const;

    private:
        std::unique_ptr<api::IMessageApi> messageApi_;
        std::unique_ptr<stx::ConfigStorage> configStorage_;
    };
}
