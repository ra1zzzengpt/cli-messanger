#pragma once
#include <expected>
#include <api/message_api/imessage_api.h>
#include <models/app_config.hpp>
#include <utils/files/config_storage/config_storage.hpp>
#include <utils/time/time_controller.hpp>

namespace app
{
    namespace
    {
        const std::string kCurrentVersion = "v1.0";
    }

    class AppController final
    {
    public:
        // - OBJECT -
        AppController(std::unique_ptr<api::IMessageApi> api,
            std::unique_ptr<stx::ConfigStorage> storage, std::unique_ptr<stx::TimeController> time);
        ~AppController() = default;
        AppController(AppController&) = delete;
        AppController(AppController&&) = delete;
        AppController& operator=(const AppController&) = delete;
        AppController& operator=(AppController&&) = delete;

        // - GETTERS FIELDS (WITHOUT OBJECTS) -

        [[nodiscard]] const std::string& getCurrentVersion() const;
        [[nodiscard]] const std::string& getLatestVersion() const;

        // - CONFIG -
        [[nodiscard]] const AppConfig& getAppConfig() const noexcept;
        [[nodiscard]] const std::vector<ChatInfo>& getChats() const;
        std::expected<void,stx::err::Error> loadAppConfig();
        std::expected<void,stx::err::Error> saveAppConfig();
        std::expected<void,stx::err::Error> setLogin(const UserInfo& user, const std::string& password);
        std::expected<void,stx::err::Error> setupInitialUser(std::uint64_t id, const std::string& nickname);
        std::expected<void,stx::err::Error> updateConfigPassword(const std::string& new_password);
        std::expected<void,stx::err::Error> updateConfigNickname(const std::string& new_nickname);
        std::expected<void,stx::err::Error> updateConfigUrl(const std::string& new_url);
        std::expected<void,stx::err::Error> addChat(const ChatInfo& new_chat);

        // - NETWORK -
        std::expected<void,stx::err::Error> updateUrl(const std::string& new_url);
        [[nodiscard]] std::expected<std::string,stx::err::Error> ping() const;
        [[nodiscard]] std::expected<std::vector<Message>,stx::err::Error> getMessages(const UserInfo& other_user) const;
        [[nodiscard]] std::expected<void,stx::err::Error> sendMessage(const UserInfo& other_user, const std::string& text) const;
        [[nodiscard]] std::expected<void,stx::err::Error> updatePassword(const std::string& new_password) const;
        [[nodiscard]] std::expected<void,stx::err::Error> updateNickname(const std::string& new_nickname) const;
        [[nodiscard]] std::expected<UserInfo,stx::err::Error> getNicknameById(std::uint64_t id) const;
        [[nodiscard]] std::expected<void,stx::err::Error> registerUser(const UserInfo& user) const;
        [[nodiscard]] std::expected<void,stx::err::Error> loginUser(std::uint64_t id, const std::string& password) const;

        // - INFO FROM NET -
        [[nodiscard]] std::expected<std::string,stx::err::Error> versionControl() const;

        // - TIME -
        bool askForRequest(std::chrono::time_point<std::chrono::steady_clock> compare);
    private:
        std::unique_ptr<api::IMessageApi> messageApi_;
        std::unique_ptr<stx::ConfigStorage> configStorage_;
        std::unique_ptr<stx::TimeController> timeController_;
        std::string current_version_ = kCurrentVersion, latest_version_;
    };
}
