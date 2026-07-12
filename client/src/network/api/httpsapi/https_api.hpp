#pragma once
#include <../i_network_api.h>
#include <network/api/message_api/httpsapi/github_api/github_api.hpp>

namespace api
{
    class HttpMessageApi : public IMessageApi
    {
    public:
        explicit HttpMessageApi(GitHubApi& github_api);
        HttpMessageApi(const HttpMessageApi&) = delete;
        HttpMessageApi& operator=(const HttpMessageApi&) = delete;
        HttpMessageApi(HttpMessageApi&&) = delete;
        HttpMessageApi& operator=(HttpMessageApi&&) = delete;

        std::expected<std::string,stx::err::Error> ping() override;

        std::expected<void,stx::err::Error> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) override;

        std::expected<void,stx::err::Error> loginUser(
            std::uint64_t id,
            const std::string& password
        ) override;

        std::expected<UserInfo,stx::err::Error> getUsernameById(
            std::uint64_t id
        ) override;
        
        std::expected<void,stx::err::Error> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) override;

        std::expected<void,stx::err::Error> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) override;

        std::expected<void,stx::err::Error> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) override;

        
        std::expected<std::vector<Message>,stx::err::Error> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) override;

        std::expected<std::string,stx::err::Error> versionControl() override;

        void setUrl(const std::string& url) override;
    private:
        std::string url_;
    };
}
