#pragma once
#include "api/message_api/imessage_api.h"
#include "models/app_config.h"
#include "utils/files/config_storage/config_storage.h"
namespace app
{
    class AppController final
    {
    public:
        AppController(std::unique_ptr<api::IMessageApi> api,
            std::unique_ptr<utils::ConfigStorage> storage);

        AppConfig& getAppConfig();
        [[nodiscard]] api::IMessageApi& getMessageApi() const;

        bool loadAppConfig();

        [[nodiscard]] bool saveAppConfig() const;

        std::vector<ChatInfo>& getChats();
        [[nodiscard]] std::optional<std::string> ping() const;
        [[nodiscard]] std::vector<Message> dumpMessages(const UserInfo& other_user) const;
        [[nodiscard]] std::vector<Message> getMessages(const UserInfo& other_user, const ChatInfo& chat) const;
        [[nodiscard]] bool sendMessage(const UserInfo& other_user, const std::string &text) const;
        [[nodiscard]] bool updatePassword(const std::string &new_password) const;
        [[nodiscard]] bool updateNickname(const std::string &new_nickname) const;
        [[nodiscard]] std::optional<UserInfo> getNicknameById(uint64_t id) const;
        [[nodiscard]] bool registerUser(const UserInfo& user) const;
        [[nodiscard]] bool loginUser(uint64_t id, const std::string& password) const;
    private:
        AppConfig config_;
        std::unique_ptr<api::IMessageApi> messageApi_;
        std::unique_ptr<utils::ConfigStorage> configStorage_;
    };
}