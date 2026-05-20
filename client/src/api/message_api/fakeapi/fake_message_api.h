#pragma once
#include <optional>
#include <cstdint>
#include <string>

#include "api/message_api/imessage_api.h"
#include "models/user_info.h"
#include "models/message.h"

namespace api {
    class FakeMessageApi final : public IMessageApi{
    public:
        FakeMessageApi() = default;

        std::expected<std::string,stx::err::AppError> ping() override;

        std::expected<void,stx::err::AppError> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) override;

        std::expected<void,stx::err::AppError> loginUser(
            std::uint64_t id,
            const std::string& password
        ) override;

        std::expected<UserInfo,stx::err::AppError> getUsernameById(
            std::uint64_t id,
            const std::string& password
        ) override;

        std::expected<void,stx::err::AppError> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) override;

        std::expected<void,stx::err::AppError> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) override;

        std::expected<void,stx::err::AppError> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) override;


        std::expected<std::vector<Message>,stx::err::AppError> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) override;

        void setUrl(const std::string& url) override;
    };
}