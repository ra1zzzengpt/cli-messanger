#include "app_controller.h"

namespace app
{
    AppController::AppController(
        std::unique_ptr<api::IMessageApi> api,
        std::unique_ptr<utils::ConfigStorage> storage
        ) : messageApi_(std::move(api)), configStorage_(std::move(storage))
    { }

    const AppConfig& AppController::getAppConfig() const noexcept
    {
        return configStorage_->getConfig();
    }

    void AppController::loadAppConfig()
    {
        configStorage_->load();
        messageApi_->setHost(configStorage_->getConfig().server.host);
        messageApi_->setPort(configStorage_->getConfig().server.port);
    }

    bool AppController::saveAppConfig()
    {
        return configStorage_->save();
    }

    void AppController::setLogin(const UserInfo &user, const std::string &password)
    {
        configStorage_->setByLogin(user, password);
    }

    void AppController::updateConfigPassword(const std::string &new_password)
    {
        configStorage_->updatePassword(new_password);
    }

    void AppController::updateConfigNickname(const std::string &new_nickname)
    {
        configStorage_->updateNickname(new_nickname);
    }

    void AppController::addChat(const ChatInfo &new_chat)
    {
        configStorage_->addChat(new_chat);
    }

    void AppController::updateConfigHost(const std::string &new_host)
    {
        configStorage_->updateHost(new_host);
    }

    void AppController::updateConfigPort(const std::string& new_port)
    {
        configStorage_->updatePort(new_port);
    }

    // --- SHELLS FOR NETWORK ---
    void AppController::updateHost(const std::string &new_host) const
    {
        messageApi_->setHost(new_host);
    }

    void AppController::updatePort(const std::string &new_port) const
    {
        messageApi_->setPort(new_port);
    }

    std::optional<std::string> AppController::ping() const
    {
        return messageApi_->ping();
    }

    const std::vector<ChatInfo>& AppController::getChats() const
    {
        return configStorage_->getConfig().chats;
    }

    std::vector<Message> AppController::getMessages(const UserInfo& other_user) const
    {
        return messageApi_->dumpMessages(configStorage_->getConfig().user.id, other_user.id);
    }

    bool AppController::sendMessage(const UserInfo& other_user, const std::string& text) const
    {
        return messageApi_->sendMessage(configStorage_->getConfig().user.id, other_user.id, configStorage_->getConfig().user.password, text);
    }

    bool AppController::updatePassword(const std::string& new_password) const
    {
        return messageApi_->updatePassword(configStorage_->getConfig().user.id, configStorage_->getConfig().user.password, new_password);
    }

    bool AppController::updateNickname(const std::string& new_nickname) const
    {
        return messageApi_->updateNickname(configStorage_->getConfig().user.id, configStorage_->getConfig().user.password, new_nickname);
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