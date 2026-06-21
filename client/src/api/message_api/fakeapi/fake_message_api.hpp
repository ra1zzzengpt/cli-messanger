#pragma once
#include <string>

#include "api/message_api/imessage_api.h"
#include "models/user_info.hpp"
#include "models/message.hpp"

namespace api
{
    // Fake api for testing
    class FakeMessageApi final : public IMessageApi
    {
    public:
        FakeMessageApi() = default;

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

        void setUrl(const std::string& url) override;
    };
}