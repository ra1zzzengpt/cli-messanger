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

        bool registerUser( // todo: to expected
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) override;

        bool loginUser( // todo: to expected
            std::uint64_t id,
            const std::string& password
        ) override;

        std::optional<UserInfo> getUsernameById( // todo: to expected
            std::uint64_t id,
            const std::string& password
        ) override;

        bool updatePassword( // todo: to expected
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) override;

        bool updateNickname( // todo: to expected
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) override;

        bool sendMessage( // todo: to expected
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) override;

        std::vector<Message> dumpMessages( // todo: to expected
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        );

        void setUrl(const std::string& url);
    };
}