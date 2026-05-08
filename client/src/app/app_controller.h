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

        AppConfig& GetAppConfig();
        [[nodiscard]] api::IMessageApi& GetMessageApi() const;

        bool LoadAppConfig();

        [[nodiscard]] bool SaveAppConfig() const;

        std::vector<ChatInfo>& GetChats();
        [[nodiscard]] std::vector<Message> GetMessages(const UserInfo& other_user, const ChatInfo& chat) const;
        [[nodiscard]] bool SendMessage(const UserInfo& other_user, const std::string &text) const;
        [[nodiscard]] bool updateNickname(const std::string &new_nickname) const;
        [[nodiscard]] std::optional<UserInfo> getNicknameById(uint64_t id) const;
        [[nodiscard]] bool registerUser(const UserInfo& user) const;
    private:
        AppConfig config;
        std::unique_ptr<api::IMessageApi> messageApi_;
        std::unique_ptr<utils::ConfigStorage> configStorage_;
    };
}