#pragma once

#include <network/api/httpsapi/https_api.hpp>
#include <network/api/github_api/github_api.hpp>
#include <utils/error/app_error.hpp>
#include <models/user_info.hpp>
#include <models/message.hpp>
#include <vector>
#include <string>
#include <expected>

namespace net
{
    class NetworkController final
    {
    public:
        NetworkController();
        ~NetworkController() = default;
        NetworkController(const NetworkController &) = delete;
        NetworkController &operator=(const NetworkController &) = delete;
        NetworkController(NetworkController &&) = delete;
        NetworkController &operator=(NetworkController &&) = delete;

        std::expected<std::string,stx::err::Error> ping();

        std::expected<void,stx::err::Error> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        );

        std::expected<void,stx::err::Error> loginUser(
            std::uint64_t id,
            const std::string& password
        );

        std::expected<UserInfo,stx::err::Error> getUsernameById(
            std::uint64_t id
        );

        std::expected<void,stx::err::Error> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        );

        std::expected<void,stx::err::Error> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        );

        std::expected<void,stx::err::Error> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        );


        std::expected<std::vector<Message>,stx::err::Error> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        );

        std::expected<std::string,stx::err::Error> versionControl();

        void setUrlMessageApi(const std::string& url);
    private:
        api::GitHubApi gitHubApi_;
        api::HttpMessageApi httpMessageApi_;
    };
}