#pragma once
#include "api/message_api/imessage_api.h"

namespace api {
    enum class HttpsError {
        CurlInitFailed   = 1,  // curl_easy_init() вернул nullptr — редкая системная ошибка
        ConnectionFailed = 2,  // соединение не установлено: сервер недоступен, нет сети
        Timeout          = 3,  // запрос превысил таймаут (5 секунд)
        JsonParseError   = 4,  // тело ответа не является валидным JSON
        InvalidResponse  = 5,  // JSON валиден, но ожидаемые поля отсутствуют
        BadRequest       = 6,  // HTTP 400 — невалидные параметры (пустой nick, короткий пароль и т.д.)
        Unauthorized     = 7,  // HTTP 401 — неверный пароль
        NotFound         = 8,  // HTTP 404 — пользователь или ресурс не найден
        Conflict         = 9,  // HTTP 409 — ресурс уже существует (например, id занят)
        ServerError      = 10, // HTTP 5xx — внутренняя ошибка сервера
        UnexpectedStatus = 11, // любой другой HTTP-статус
    };

    HttpsError ErrorCodeToHttpsErrors(long statusCode);

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
