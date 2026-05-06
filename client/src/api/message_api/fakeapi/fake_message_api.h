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

        bool registerUser(
            std::uint64_t id,
            const std::string& nick
        ) override;

        std::optional<UserInfo> getUsernameById(
            std::uint64_t id
        ) override;

        bool updateNickname(
            std::uint64_t id,
            const std::string& newNick
        ) override;

        bool sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& text
        ) override;

        std::vector<Message> fetchMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            uint64_t sinceMessageId
        ) override;
    };
}