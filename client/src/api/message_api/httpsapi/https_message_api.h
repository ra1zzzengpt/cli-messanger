#pragma once
#include "api/message_api/imessage_api.h"

namespace api {
    class HttpMessageApi : public IMessageApi {
    public:
        HttpMessageApi() = default;

        std::optional<std::string> ping() override;

        bool registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) override;

        bool loginUser(
            std::uint64_t id,
            const std::string& password
        ) override;

        std::optional<UserInfo> getUsernameById(
            std::uint64_t id
        ) override;
        
        bool updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) override;

        bool updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) override;

        bool sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) override;

        
        std::vector<Message> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId
        ) override;

        std::vector<Message> fetchMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            uint64_t sinceMessageId
        ) override;

        void setHost(const std::string& host) override;
        void setPort(const std::string& port) override;
    private:
        std::string host_;
        std::string port_;
        [[nodiscard]] std::string to_url() const;
    };
}
