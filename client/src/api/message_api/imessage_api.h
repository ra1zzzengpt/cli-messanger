#pragma once
#include <expected>
#include <string>
#include <vector>
#include <models/user_info.hpp>
#include <models/message.hpp>
#include <utils/error/app_error.hpp>

// ABOUT USING PASSWORD IN REQUESTS:
// At the moment, we are just checking the user's ability to use this or that function.
// In future updates, we will try to SWITCH TO TOKENS (WEBSOCKET/TCP).

namespace api
{
    class IMessageApi
    {
    public:
        virtual ~IMessageApi() = default;

        virtual std::expected<std::string,stx::err::Error> ping() = 0;

        virtual std::expected<void,stx::err::Error> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) = 0;

        virtual std::expected<void,stx::err::Error> loginUser(
            std::uint64_t id,
            const std::string& password
        ) = 0;

        virtual std::expected<UserInfo,stx::err::Error> getUsernameById(
            std::uint64_t id
        ) = 0;
        
        virtual std::expected<void,stx::err::Error> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) = 0;

        virtual std::expected<void,stx::err::Error> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) = 0;

        virtual std::expected<void,stx::err::Error> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) = 0;
        
        virtual std::expected<std::vector<Message>,stx::err::Error> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) = 0;

        virtual void setUrl(const std::string& url) = 0;
    };
}
