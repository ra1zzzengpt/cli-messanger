#include "app_controller.h"
#include <nlohmann/json.hpp>

namespace app
{
    AppController::AppController(
        std::unique_ptr<api::IMessageApi> api,
        std::unique_ptr<utils::ConfigStorage> storage
        ) : messageApi_(std::move(api)), configStorage_(std::move(storage))
    { }

    AppConfig& AppController::getAppConfig()
    {
        return config_;
    }

    api::IMessageApi& AppController::getMessageApi() const
    {
        return *messageApi_;
    }

    bool AppController::loadAppConfig()
    {
        std::optional<AppConfig> opt = configStorage_->load();
        if (!opt.has_value())
            return false;
        config_ = std::move(opt.value());
        messageApi_->setHost(config_.server.host);
        messageApi_->setPort(std::to_string(config_.server.port));
        return true;
    }

    bool AppController::saveAppConfig() const
    {
        return configStorage_->save(config_);
    }

    // --- SHELLS FOR NETWORK ---
    std::optional<std::string> AppController::ping() const
    {
        return messageApi_->ping();
    }

    std::vector<ChatInfo>& AppController::getChats()
    {
        return config_.chats;
    }
    
    std::vector<Message> AppController::dumpMessages(const UserInfo& other_user) const
    {
        return messageApi_->dumpMessages(config_.user.id, other_user.id);
    }

    std::vector<Message> AppController::getMessages(const UserInfo& other_user, const ChatInfo& chat) const
    {
        return messageApi_->fetchMessages(config_.user.id,other_user.id,chat.last_message_id);
    }

    bool AppController::sendMessage(const UserInfo& other_user, const std::string& text) const
    {
        return messageApi_->sendMessage(config_.user.id, other_user.id, config_.user.password, text);
    }

    bool AppController::updatePassword(const std::string& new_password) const
    {
        return messageApi_->updatePassword(config_.user.id, config_.user.password, new_password);
    }

    bool AppController::updateNickname(const std::string& new_nickname) const
    {
        return messageApi_->updateNickname(config_.user.id, config_.user.password, new_nickname);
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