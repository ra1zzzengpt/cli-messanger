#pragma once
#include "api/message_api/imessage_api.h"

namespace api {
    // HttpsError ErrorCodeToHttpsErrors(long statusCode);

    class HttpMessageApi : public IMessageApi {
    public:
        HttpMessageApi() = default;

        std::optional<std::string> ping() override;  // todo: to request body

        bool registerUser(  // todo: to request body
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) override;

        bool loginUser(  // todo: to request body
            std::uint64_t id,
            const std::string& password
        ) override;

        std::optional<UserInfo> getUsernameById(  // todo: to request body
            std::uint64_t id,
            const std::string& password
        ) override;
        
        bool updatePassword(  // todo: to request body
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) override;

        bool updateNickname(  // todo: to request body
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) override;

        bool sendMessage(  // todo: to request body
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) override;

        
        std::vector<Message> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) override;

        void setUrl(const std::string& url) override;
    private:
        std::string url_;
    };
}
