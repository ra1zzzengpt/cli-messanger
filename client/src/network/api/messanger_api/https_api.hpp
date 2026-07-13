#pragma once

#include <string>
#include <expected>
#include <utils/error/app_error.hpp>
#include <vector>
#include <models/user_info.hpp>
#include <models/message.hpp>

namespace net::api
{
    class MessangerApi
    {
    public:
        [[nodiscard]] std::expected<std::string,stx::err::Error> ping() const;

        [[nodiscard]] std::expected<void,stx::err::Error> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) const;

        [[nodiscard]] std::expected<void,stx::err::Error> loginUser(
            std::uint64_t id,
            const std::string& password
        ) const;

        [[nodiscard]] std::expected<UserInfo,stx::err::Error> getUsernameById(
            std::uint64_t id
        ) const;
        
        [[nodiscard]] std::expected<void,stx::err::Error> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) const;

        [[nodiscard]] std::expected<void,stx::err::Error> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) const;

        [[nodiscard]] std::expected<void,stx::err::Error> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) const;

        [[nodiscard]] std::expected<std::vector<Message>,stx::err::Error> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) const;

        void setUrl(const std::string& url);
    private:
        std::string url_;
    };
}
