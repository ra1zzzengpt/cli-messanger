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

        void LoadAppConfig();

        [[nodiscard]] bool SaveAppConfig() const;

        // todo make shells for Net
        // std::vector<ChatInfo> GetChats();
        // std::vector<Message> GetMessages();
        // void SendMessage(std::uint64_t chatId, const std::string &text);
    private:
        AppConfig config;
        std::unique_ptr<api::IMessageApi> messageApi_;
        std::unique_ptr<utils::ConfigStorage> configStorage_;
    };
}