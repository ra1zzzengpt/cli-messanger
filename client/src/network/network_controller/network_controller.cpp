#include "network_controller.hpp"


namespace net
{
    // ------------------------------ MESSANGER API ----------------------------------
    std::expected<std::string,stx::err::Error> NetworkController::ping() const
    {
        return httpMessageApi_.ping();
    }

    std::expected<std::string, stx::err::Error> NetworkController::ping(const std::string &another_url) const
    {
        return api::MessangerApi::ping(another_url);
    }

    std::expected<void, stx::err::Error> NetworkController::loginUser(const std::uint64_t id, const std::string &password) const
    {
        return httpMessageApi_.loginUser(id, password);
    }

    std::expected<std::string, stx::err::Error> NetworkController::getUsernameById(const std::uint64_t id) const
    {
        return httpMessageApi_.getUsernameById(id);
    }

    std::expected<void, stx::err::Error> NetworkController::registerUser(const std::uint64_t id, const std::string &nick, const std::string &password) const
    {
        return httpMessageApi_.registerUser(id, nick, password);
    }

    std::expected<void, stx::err::Error> NetworkController::sendMessage(const std::uint64_t fromId, const std::uint64_t toId, const std::string &password, const std::string &text) const
    {
        return httpMessageApi_.sendMessage(fromId, toId, password, text);
    }

    std::expected<void, stx::err::Error> NetworkController::updateNickname(const std::uint64_t id, const std::string &password, const std::string &newNick) const
    {
        return httpMessageApi_.updateNickname(id, password, newNick);
    }

    std::expected<void, stx::err::Error> NetworkController::updatePassword(const std::uint64_t id, const std::string &currentPassword, const std::string &newPassword) const
    {
        return httpMessageApi_.updatePassword(id, currentPassword, newPassword);
    }

    std::expected<std::vector<Message>, stx::err::Error> NetworkController::dumpMessages(const std::uint64_t myId, const std::uint64_t peerId, const std::string &password) const
    {
        return httpMessageApi_.dumpMessages(myId, peerId, password);
    }

    void NetworkController::setUrlMessageApi(const std::string &url)
    {
        httpMessageApi_.setUrl(url);
    }

    // ------------------------------------- GITHUB API ---------------------------------------

    bool NetworkController::verifyVersion() const
    {
        return gitHubApi_.lastestVersionControl();
    }

    const std::string &NetworkController::currentVersion() const
    {
        return gitHubApi_.currentVersion();
    }

    const std::string &NetworkController::lastestVersion() const
    {
        return gitHubApi_.lastestVersion();
    }
}
