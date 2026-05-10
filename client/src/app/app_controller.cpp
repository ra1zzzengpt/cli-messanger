#include "app_controller.h"
#include <nlohmann/json.hpp>

namespace app
{
    AppController::AppController(
        std::unique_ptr<api::IMessageApi> api,
        std::unique_ptr<utils::ConfigStorage> storage
        ) : messageApi_(std::move(api)), configStorage_(std::move(storage))
    { }

    AppConfig& AppController::GetAppConfig()
    {
        return config;
    }

    api::IMessageApi& AppController::GetMessageApi() const
    {
        return *messageApi_;
    }

    bool AppController::LoadAppConfig()
    {
        std::optional<AppConfig> opt = configStorage_->Load();
        if (!opt.has_value())
            return false;
        config = std::move(opt.value());
        messageApi_->set_host(config.server.host);
        messageApi_->set_port(std::to_string(config.server.port));
        return true;
    }

    bool AppController::SaveAppConfig() const
    {
        return configStorage_->Save(config);
    }

    // --- SHELLS FOR NETWORK ---
    std::optional<std::string> AppController::Ping() const
    {
        return messageApi_->ping();
    }

    std::vector<ChatInfo>& AppController::GetChats()
    {
        return config.chats;
    }
    
    std::vector<Message> AppController::DumpMessages(const UserInfo& other_user) const
    {
        return messageApi_->dumpMessages(config.user.id, other_user.id);
    }

    std::vector<Message> AppController::GetMessages(const UserInfo& other_user, const ChatInfo& chat) const
    {
        return messageApi_->fetchMessages(config.user.id,other_user.id,chat.last_message_id);
    }

    bool AppController::SendMessage(const UserInfo& other_user, const std::string& text) const
    {
        return messageApi_->sendMessage(config.user.id, other_user.id, config.user.password, text);
    }

    bool AppController::updatePassword(const std::string& new_password) const
    {
        return messageApi_->updatePassword(config.user.id, config.user.password, new_password);
    }

    bool AppController::updateNickname(const std::string& new_nickname) const
    {
        return messageApi_->updateNickname(config.user.id, config.user.password, new_nickname);
    }

    std::optional<UserInfo> AppController::getNicknameById(const uint64_t id) const
    {
        return messageApi_->getUsernameById(id);
    }

    bool AppController::registerUser(const UserInfo &user) const
    {
        return messageApi_->registerUser(user.id,user.nickname,user.password);
    }

    bool AppController::loginUser(const uint64_t id, const std::string& password) const
    {
        return messageApi_->loginUser(id, password);
    }
}