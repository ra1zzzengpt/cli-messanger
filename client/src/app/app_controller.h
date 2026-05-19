#pragma once
#include "api/message_api/imessage_api.h"
#include "models/app_config.h"
#include "utils/files/config_storage/config_storage.h"
namespace app
{
    class AppController final
    {
    public:
        // - OBJ -
        AppController(std::unique_ptr<api::IMessageApi> api,
            std::unique_ptr<utils::ConfigStorage> storage);
        ~AppController() = default;
        AppController(AppController&&) = delete;
        AppController& operator=(const AppController&) = delete;
        AppController& operator=(AppController&&) = delete;

        // - CONFIG -
        [[nodiscard]] const AppConfig& getAppConfig() const noexcept;
        void loadAppConfig();
        [[nodiscard]] bool saveAppConfig();
        void setLogin(const UserInfo& user, const std::string& password);
        void updateConfigPassword(const std::string &new_password);
        void updateConfigNickname(const std::string& new_nickname);
        void updateConfigUrl(const std::string& new_url);
        [[nodiscard]] const std::vector<ChatInfo>& getChats() const;
        void addChat(const ChatInfo& new_chat);

        // - NET -
        void updateUrl(const std::string& new_url) const;
        [[nodiscard]] std::optional<std::string> ping() const;
        [[nodiscard]] std::vector<Message> getMessages(const UserInfo& other_user) const;
        [[nodiscard]] bool sendMessage(const UserInfo& other_user, const std::string &text) const;
        [[nodiscard]] bool updatePassword(const std::string &new_password) const;
        [[nodiscard]] bool updateNickname(const std::string &new_nickname) const;
        [[nodiscard]] std::optional<UserInfo> getNicknameById(uint64_t id) const;
        [[nodiscard]] bool registerUser(const UserInfo& user) const;
        [[nodiscard]] bool loginUser(uint64_t id, const std::string& password) const;
    private:
        std::unique_ptr<api::IMessageApi> messageApi_;
        std::unique_ptr<utils::ConfigStorage> configStorage_;
    };
}