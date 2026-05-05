#pragma once
#include <optional>
#include <cstdint>
#include <string>
#include "models/user_info.h"
#include "models/message.h"

namespace api {
    class IMessageApi {
    public:
        virtual ~IMessageApi() = default;

        virtual bool registerUser(
            std::uint64_t id,
            const std::string& nick
        ) = 0;

        virtual std::optional<UserInfo> getUserById(
            std::uint64_t id
        ) = 0;

        virtual bool updateNick(
            std::uint64_t id,
            const std::string& newNick
        ) = 0;

        virtual bool sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& text
        ) = 0;

        virtual std::vector<Message> fetchMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            int sinceMessageId
        ) = 0;
    };
}