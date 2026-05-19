#pragma once
#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include "models/user_info.h"
#include "models/message.h"

namespace api
{
    class IMessageApi
    {
    public:
        virtual ~IMessageApi() = default;

        virtual std::optional<std::string> ping() = 0;

        virtual bool registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) = 0;

        virtual bool loginUser(
            std::uint64_t id,
            const std::string& password
        ) = 0;

        virtual std::optional<UserInfo> getUsernameById(
            std::uint64_t id,
            const std::string& password
        ) = 0;
        
        virtual bool updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) = 0;

        virtual bool updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) = 0;

        virtual bool sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) = 0;
        
        virtual std::vector<Message> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) = 0;

        virtual void setUrl(const std::string& url) = 0;
    };
}
