#pragma once
#include "api/message_api/imessage_api.h"

namespace api {
    // HttpsError ErrorCodeToHttpsErrors(long statusCode);

    class HttpMessageApi : public IMessageApi {
    public:
        HttpMessageApi() = default;

        std::expected<std::string,utils::errors::AppError> ping() override;

        std::expected<void,utils::errors::AppError> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) override;

        std::expected<void,utils::errors::AppError> loginUser(
            std::uint64_t id,
            const std::string& password
        ) override;

        std::expected<UserInfo,utils::errors::AppError> getUsernameById(
            std::uint64_t id,
            const std::string& password
        ) override;
        
        std::expected<void,utils::errors::AppError> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) override;

        std::expected<void,utils::errors::AppError> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) override;

        std::expected<void,utils::errors::AppError> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) override;

        
        std::expected<std::vector<Message>,utils::errors::AppError> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) override;

        void setUrl(const std::string& url) override;
    private:
        std::string url_;
    };
}
