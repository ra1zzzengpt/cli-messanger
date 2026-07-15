#include <app/app_controller.hpp>
#include <expected>

namespace app
{
    AppController::AppController(
        std::unique_ptr<net::NetworkController> net,
        std::unique_ptr<stx::StorageController> storage,
        std::unique_ptr<stx::TimeController> time
    ) : networkController_(std::move(net)), storageController_(std::move(storage)), timeController_(std::move(time))
    { }

    // - - - U P D A T E R S - - -

    std::expected<void,stx::err::Error> AppController::updateUrl(const std::string& new_url)
    {
        networkController_->setUrlMessageApi(new_url);
        return storageController_->updateUrl(new_url);
    }

    std::expected<void,stx::err::Error> AppController::updatePassword(const std::string& new_password) const
    {
        const auto& cfg = storageController_->getConfig().user;
        if (const std::expected<void,stx::err::Error> net_update = networkController_->updatePassword(cfg.id, cfg.password, new_password); !net_update.has_value())
        {
            return net_update;
        }
        if (const std::expected<void,stx::err::Error> storage_update = storageController_->updatePassword(new_password); !storage_update.has_value())
        {
            return storage_update;
        }
        return {};
    }

    std::expected<void,stx::err::Error> AppController::updateNickname(const std::string& new_nickname) const
    {
        const auto& cfg = storageController_->getConfig().user;
        if (const std::expected<void,stx::err::Error> net_update = networkController_->updateNickname(cfg.id, cfg.password, new_nickname); !net_update.has_value())
        {
            return net_update;
        }
        if (const std::expected<void,stx::err::Error> storage_update = storageController_->updateNickname(new_nickname); !storage_update.has_value())
        {
            return storage_update;
        }
        return {};
    }

    std::expected<void, stx::err::Error> AppController::updateID(const std::uint64_t& new_id) const
    {
        return storageController_->updateID(new_id);
    }

    std::expected<void,stx::err::Error> AppController::relogging(const std::string &new_url)
    {
        storageController_->clear();
        networkController_->setUrlMessageApi(new_url);
        return storageController_->updateUrl(new_url);
    }

    // - - - C O N F I G - - -

    const AppConfig& AppController::getAppConfig() const noexcept
    {
        return storageController_->getConfig();
    }

    const std::vector<ChatInfo>& AppController::getChats() const
    {
        return storageController_->getConfig().chats;
    }

    // non const method (the body of the internal function changes the application parameters in memory)
    std::expected<void,stx::err::Error> AppController::loadAppConfig()
    {
        if (const std::expected<void,stx::err::Error> result = storageController_->load(); !result.has_value())
            return std::unexpected(result.error());
        networkController_->setUrlMessageApi(storageController_->getConfig().server.url);
        return {};
    }

    // non const also
    std::expected<void,stx::err::Error> AppController::saveAppConfig()
    {
        return storageController_->save();
    }

    // non const also
    std::expected<void,stx::err::Error> AppController::setLogin(const UserInfo& user, const std::string& password)
    {
        return storageController_->setByLogin(user, password);
    }

    // non const also
    std::expected<void,stx::err::Error> AppController::addChat(const ChatInfo& new_chat)
    {
        return storageController_->addChat(new_chat);
    }

    // - - - N E T W O R K - - -
    std::expected<std::string,stx::err::Error> AppController::ping() const
    {
        return networkController_->ping();
    }

    std::expected<std::string, stx::err::Error> AppController::ping(const std::string &another_url) const
    {
        return networkController_->ping(another_url);
    }

    std::expected<std::vector<Message>,stx::err::Error> AppController::getMessages(const UserInfo& other_user) const
    {
        const auto& cfg = storageController_->getConfig().user;
        return networkController_->dumpMessages(cfg.id, other_user.id, cfg.password);
    }

    std::expected<void,stx::err::Error> AppController::sendMessage(const UserInfo& other_user, const std::string& text) const
    {
        const auto& cfg = storageController_->getConfig().user;
        return networkController_->sendMessage(cfg.id, other_user.id, cfg.password, text);
    }

    std::expected<UserInfo,stx::err::Error> AppController::getNicknameById(const std::uint64_t id) const
    {
        return networkController_->getUsernameById(id);
    }

    std::expected<void,stx::err::Error> AppController::registerUser(const UserInfo& user) const
    {
        return networkController_->registerUser(user.id, user.nickname, user.password);
    }

    std::expected<void,stx::err::Error> AppController::loginUser(const std::uint64_t id, const std::string& password) const
    {
        return networkController_->loginUser(id, password);
    }

    // - - - F R O M   N E T - - -

    bool AppController::versionControl() const
    {
        return networkController_->verifyVersion();
    }

    const std::string& AppController::currentVersion() const
    {
        return networkController_->currentVersion();
    }

    const std::string& AppController::lastestVersion() const
    {
        return networkController_->lastestVersion();
    }

    // - - - T I M E - - -
    bool AppController::canMakeRequest() const
    {
        return timeController_->canMakeRequest();
    }

    bool AppController::tryAcquireRequest()
    {
        return timeController_->tryAcquireRequest();
    }
}
