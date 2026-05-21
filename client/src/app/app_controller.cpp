#include "app_controller.h"

namespace app
{
    AppController::AppController(
        std::unique_ptr<api::IMessageApi> api,
        std::unique_ptr<stx::ConfigStorage> storage
    ) : messageApi_(std::move(api)), configStorage_(std::move(storage)) {}

    // ── Config ────────────────────────────────────────────────────────────────

    const AppConfig& AppController::getAppConfig() const noexcept
    {
        return configStorage_->getConfig();
    }

    const std::vector<ChatInfo>& AppController::getChats() const
    {
        return configStorage_->getConfig().chats;
    }

    std::expected<void,stx::err::AppError> AppController::loadAppConfig()
    {
        if (const std::expected<void,stx::err::AppError> result = configStorage_->load(); !result.has_value())
            return std::unexpected(result.error());
        messageApi_->setUrl(configStorage_->getConfig().server.url);
        return {};
    }

    std::expected<void,stx::err::AppError> AppController::saveAppConfig()
    {
        return configStorage_->save();
    }

    std::expected<void,stx::err::AppError> AppController::setLogin(const UserInfo& user, const std::string& password)
    {
        return configStorage_->setByLogin(user, password);
    }

    std::expected<void,stx::err::AppError> AppController::setupInitialUser(const std::uint64_t id, const std::string& nickname)
    {
        return configStorage_->setInitialUser(id, nickname);
    }

    std::expected<void,stx::err::AppError> AppController::updateConfigPassword(const std::string& new_password)
    {
        return configStorage_->updatePassword(new_password);
    }

    std::expected<void,stx::err::AppError> AppController::updateConfigNickname(const std::string& new_nickname)
    {
        return configStorage_->updateNickname(new_nickname);
    }

    std::expected<void,stx::err::AppError> AppController::updateConfigUrl(const std::string& new_url)
    {
        return configStorage_->updateUrl(new_url);
    }

    std::expected<void,stx::err::AppError> AppController::addChat(const ChatInfo& new_chat)
    {
        return configStorage_->addChat(new_chat);
    }

    // ── Net ───────────────────────────────────────────────────────────────────

    // Updates both the live API handle and the persisted config
    std::expected<void,stx::err::AppError> AppController::updateUrl(const std::string& new_url)
    {
        messageApi_->setUrl(new_url);
        return configStorage_->updateUrl(new_url);
    }

    std::expected<std::string,stx::err::AppError> AppController::ping() const
    {
        return messageApi_->ping();
    }

    std::expected<std::vector<Message>,stx::err::AppError> AppController::getMessages(const UserInfo& other_user) const
    {
        const auto& cfg = configStorage_->getConfig().user;
        return messageApi_->dumpMessages(cfg.id, other_user.id, cfg.password);
    }

    std::expected<void,stx::err::AppError> AppController::sendMessage(const UserInfo& other_user, const std::string& text) const
    {
        const auto& cfg = configStorage_->getConfig().user;
        return messageApi_->sendMessage(cfg.id, other_user.id, cfg.password, text);
    }

    std::expected<void,stx::err::AppError> AppController::updatePassword(const std::string& new_password) const
    {
        const auto& cfg = configStorage_->getConfig().user;
        return messageApi_->updatePassword(cfg.id, cfg.password, new_password);
    }

    std::expected<void,stx::err::AppError> AppController::updateNickname(const std::string& new_nickname) const
    {
        const auto& cfg = configStorage_->getConfig().user;
        return messageApi_->updateNickname(cfg.id, cfg.password, new_nickname);
    }

    std::expected<UserInfo,stx::err::AppError> AppController::getNicknameById(const std::uint64_t id) const
    {
        return messageApi_->getUsernameById(id, configStorage_->getConfig().user.password);
    }

    std::expected<void,stx::err::AppError> AppController::registerUser(const UserInfo& user) const
    {
        return messageApi_->registerUser(user.id, user.nickname, user.password);
    }

    std::expected<void,stx::err::AppError> AppController::loginUser(const std::uint64_t id, const std::string& password) const
    {
        return messageApi_->loginUser(id, password);
    }
}