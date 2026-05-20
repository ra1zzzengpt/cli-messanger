#pragma once
#include <cstdint>
#include <expected>
#include <string>
#include <vector>
#include "models/user_info.h"
#include "models/message.h"
#include "utils/error/app_error.h"

namespace api
{
    class IMessageApi
    {
    public:
        virtual ~IMessageApi() = default;

        virtual std::expected<std::string,utils::errors::AppError> ping() = 0;

        virtual std::expected<void,utils::errors::AppError> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) = 0;

        virtual std::expected<void,utils::errors::AppError> loginUser(
            std::uint64_t id,
            const std::string& password
        ) = 0;

        virtual std::expected<UserInfo,utils::errors::AppError> getUsernameById(
            std::uint64_t id,
            const std::string& password
        ) = 0;
        
        virtual std::expected<void,utils::errors::AppError> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) = 0;

        virtual std::expected<void,utils::errors::AppError> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) = 0;

        virtual std::expected<void,utils::errors::AppError> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) = 0;
        
        virtual std::expected<std::vector<Message>,utils::errors::AppError> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) = 0;

        virtual void setUrl(const std::string& url) = 0;
    };
}
