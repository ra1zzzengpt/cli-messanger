#pragma once
#include "api/message_api/imessage_api.h"

namespace api {
    class HttpMessageApi : public IMessageApi{
    public:
        HttpMessageApi() = default;

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

        void set_host(const std::string& host) override;
        void set_port(const std::string& port) override;
    private:
        std::string host_;
        std::string port_;
        [[nodiscard]] std::string to_url() const;
    };
}
