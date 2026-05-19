#include "https_message_api.h"

#include <curl/curl.h>

#include "https_response.h"

namespace api {
    namespace
    {
        size_t WriteCallback (const char* ptr, const size_t size, const size_t nmemb, void* userdata) {
            const size_t real_size = size * nmemb;
            if (auto* buffer = static_cast<std::string*>(userdata)) {
                buffer->append(ptr, real_size);
                return real_size;
            }
            return 0;
        }

        void ParseResponse (const std::string& buffer, HttpResponse& response) {
            try {
                if (!buffer.empty()) {
                    response.data = nlohmann::json::parse(buffer);
                }
            } catch (const nlohmann::json::parse_error& e) {
                response.error_details = "JSON parse error: " + std::string(e.what());
            }
        }

        std::string urlEncode(const std::string& value) {
            CURL* handle = curl_easy_init();
            if (!handle) return value;
            char* encoded = curl_easy_escape(handle, value.c_str(), static_cast<int>(value.size()));
            std::string result = encoded ? encoded : value;
            if (encoded) curl_free(encoded);
            curl_easy_cleanup(handle);
            return result;
        }

        void curlEasySetoptPrepare(CURL* handle,const std::string& url,std::string& buffer)
        {
            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER,0L);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST,0L);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);
        }

        HttpResponse GET (const std::string& url) {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle)
            {
                response.error_details = "curl_easy_init failed";
                return response;
            }

            std::string buffer;
            curlEasySetoptPrepare(handle,url,buffer);

            if (const CURLcode res = curl_easy_perform(handle); res != CURLE_OK) {
                response.error_details = curl_easy_strerror(res);
            } else {
                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.status_code);
                ParseResponse(buffer, response);
            }

            curl_easy_cleanup(handle);
            return response;
        }

        HttpResponse POST (const std::string& url, const std::string& json_body) {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle)
            {
                response.error_details = "curl_easy_init failed";
                return response;
            }

            std::string buffer;
            curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curlEasySetoptPrepare(handle,url,buffer);
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_body.c_str());
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

            if (const CURLcode res = curl_easy_perform(handle); res != CURLE_OK) {
                response.error_details = curl_easy_strerror(res);
            } else {
                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.status_code);
                ParseResponse(buffer, response);
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(handle);
            return response;
        }

        HttpResponse PATCH(const std::string& url, const std::string& json_body) {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle)
            {
                response.error_details = "curl_easy_init failed";
                return response;
            }

            std::string buffer;
            curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curlEasySetoptPrepare(handle,url,buffer);
            curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_body.c_str());
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);

            if (const CURLcode res = curl_easy_perform(handle); res != CURLE_OK) {
                response.error_details = curl_easy_strerror(res);
            } else {
                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.status_code);
                ParseResponse(buffer, response);
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(handle);
            return response;
        }
    }

    // HttpsError ErrorCodeToHttpsErrors(const long statusCode) {
    //     switch (statusCode) {
    //         case 400: return HttpsError::BadRequest;
    //         case 401: return HttpsError::Unauthorized;
    //         case 404: return HttpsError::NotFound;
    //         case 409: return HttpsError::Conflict;
    //         default:
    //             if (statusCode >= 500) return HttpsError::ServerError;
    //             return HttpsError::UnexpectedStatus;
    //     }
    // }

    std::optional<std::string> HttpMessageApi::ping() // todo: to expected
    {
        if (const HttpResponse resp = GET(url_ + "/ping"); resp.is_ok() && resp.data.value("ok", false))
        {
            std::string result;
            result += "status: " + resp.data["status"].get<std::string>();
            result += " uptime: " + resp.data["uptime"].get<std::string>();
            return result;
        }
        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен или нет сети
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет полей "status" или "uptime"
        // TODO: (10) ServerError     — сервер вернул 5xx
        return std::nullopt;
    }

    bool HttpMessageApi::registerUser(const std::uint64_t id, const std::string& nick, const std::string& password) { // todo: to expected
        const std::string url = url_ + "/users/register";

        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["nick"] = nick;
        body["password"] = password;

        const HttpResponse resp = POST(url, body.dump());

        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет поля "ok" в ответе
        // TODO: (6) BadRequest       — невалидный id, пустой nick, или пароль короче 4 символов
        // TODO: (9) Conflict         — пользователь с таким id уже зарегистрирован
        // TODO: (10) ServerError     — сервер вернул 5xx
        //
        // ── Как будет выглядеть с std::expected ──────────────────────────────────
        //
        // Сигнатура:
        //   std::expected<void, HttpsError> registerUser(uint64_t, const string&, const string&)
        //
        // Внутри функции, вместо последней строки:
        //
        //   // 1. curl не дошёл до HTTP (status_code == 0, error_details непустой)
        //   if (resp.status_code == 0) {
        //       if (resp.error_details == "curl_easy_init failed")
        //           return std::unexpected(HttpsError::CurlInitFailed);          // (1)
        //       if (resp.error_details.find("timed out") != std::string::npos)
        //           return std::unexpected(HttpsError::Timeout);                 // (3)
        //       return std::unexpected(HttpsError::ConnectionFailed);            // (2)
        //   }
        //   // 2. HTTP-ответ пришёл, но JSON не распарсился (error_details непустой, status_code есть)
        //   if (!resp.error_details.empty())
        //       return std::unexpected(HttpsError::JsonParseError);              // (4)
        //   // 3. HTTP-ошибка (4xx / 5xx) — ErrorCodeToHttpsErrors даёт конкретный код
        //   if (!resp.is_ok())
        //       return std::unexpected(ErrorCodeToHttpsErrors(resp.status_code)); // (6)(9)(10)(11)
        //   // 4. Статус 2xx, но сервер вернул "ok": false или поля нет вовсе
        //   if (!resp.data.value("ok", false))
        //       return std::unexpected(HttpsError::InvalidResponse);             // (5)
        //   // 5. Всё хорошо
        //   return {};
        //
        // ── Как ловить на стороне вызывающего (app_controller): ─────────────────
        //
        //   auto result = api_->registerUser(id, nick, password);
        //   if (!result) {
        //       switch (result.error()) {
        //           case HttpsError::CurlInitFailed:
        //           case HttpsError::ConnectionFailed: // "Нет соединения с сервером"
        //           case HttpsError::Timeout:          // "Сервер не отвечает"
        //           case HttpsError::BadRequest:       // "Неверные данные (nick/пароль)"
        //           case HttpsError::Conflict:         // "Этот ID уже занят"
        //           case HttpsError::JsonParseError:
        //           case HttpsError::InvalidResponse:  // "Некорректный ответ сервера"
        //           case HttpsError::ServerError:      // "Ошибка на сервере"
        //           default: break;
        //       }
        //       return;
        //   }
        //   // result.value() — void, регистрация прошла успешно
        // ─────────────────────────────────────────────────────────────────────────
        return resp.is_ok() && resp.data.value("ok", false);
    }

    bool HttpMessageApi::loginUser(const std::uint64_t id, const std::string& password) { // todo: to expected
        const std::string url = url_ + "/users/login";

        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;

        const HttpResponse resp = POST(url, body.dump());

        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет поля "ok" в ответе
        // TODO: (6) BadRequest       — невалидный id или отсутствует пароль
        // TODO: (7) Unauthorized     — неверный пароль
        // TODO: (8) NotFound         — пользователь с таким id не найден
        return resp.is_ok() && resp.data.value("ok", false);
    }

    std::optional<UserInfo> HttpMessageApi::getUsernameById(const std::uint64_t id, const std::string& password) { // todo: to expected
        const std::string url = url_ + "/users/" + std::to_string(id) + "?password=" + urlEncode(password);  // todo: to request body
        if (auto resp = GET(url); resp.is_ok() && resp.data.value("ok", false)) {
            UserInfo info;
            info.id = std::stoull(resp.data["user"]["id"].get<std::string>());
            info.nickname = resp.data["user"]["nick"].get<std::string>();
            return info;
        }
        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет полей "user", "id" или "nick" в ответе
        // TODO: (6) BadRequest       — невалидный user id в URL
        // TODO: (7) Unauthorized     — неверный пароль
        // TODO: (8) NotFound         — пользователь с таким id не найден
        return std::nullopt;
    }

    
    bool HttpMessageApi::updatePassword(const std::uint64_t id, const std::string& currentPassword, const std::string& newPassword) { // todo: to expected
        nlohmann::json body;
        body["old_password"] = currentPassword;
        body["password"] = newPassword;
        const HttpResponse resp = PATCH(url_ + "/users/" + std::to_string(id) + "/password", body.dump());
        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет поля "ok" в ответе
        // TODO: (6) BadRequest       — новый пароль короче 4 символов
        // TODO: (7) Unauthorized     — неверный текущий пароль
        // TODO: (8) NotFound         — пользователь с таким id не найден
        return resp.is_ok() && resp.data.value("ok", false);
    }

    bool HttpMessageApi::updateNickname(const std::uint64_t id, const std::string& password, const std::string& newNick) { // todo: to expected
        nlohmann::json body;
        body["password"] = password;
        body["nick"] = newNick;
        const HttpResponse resp = PATCH(url_ + "/users/" + std::to_string(id) + "/nick", body.dump());
        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет поля "ok" в ответе
        // TODO: (6) BadRequest       — пустой nick или длиннее 32 символов
        // TODO: (7) Unauthorized     — неверный пароль
        // TODO: (8) NotFound         — пользователь с таким id не найден
        return resp.is_ok() && resp.data.value("ok", false);
    }

    bool HttpMessageApi::sendMessage(const std::uint64_t fromId, const std::uint64_t toId, const std::string& password, const std::string& text) { // todo: to expected
        nlohmann::json body;
        body["from_id"] = std::to_string(fromId);
        body["to_id"] = std::to_string(toId);
        body["password"] = password;
        body["text"] = text;
        const HttpResponse resp = POST(url_ + "/messages/send", body.dump());
        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет поля "ok" в ответе
        // TODO: (6) BadRequest       — пустой текст, from_id == to_id, или текст > 4000 символов
        // TODO: (7) Unauthorized     — неверный пароль отправителя
        // TODO: (8) NotFound         — отправитель или получатель не найден
        return resp.is_ok() && resp.data.value("ok", false);
    }

    
    std::vector<Message> HttpMessageApi::dumpMessages(const std::uint64_t myId, const std::uint64_t peerId,const std::string& password) { // todo: to expected
        const std::string url = url_ + "/messages/dump?me=" + std::to_string(myId) +
                          "&peer=" + std::to_string(peerId) +
                          "&password=" + urlEncode(password); // todo: to request body
        HttpResponse resp = GET(url);

        std::vector<Message> messages;
        if (resp.is_ok() && resp.data.value("ok", false)) {
            for (const auto& item : resp.data["messages"]) {
                Message msg;
                msg.id = std::stoull(item["id"].get<std::string>());
                msg.from_id = std::stoull(item["from_id"].get<std::string>());
                msg.to_id = std::stoull(item["to_id"].get<std::string>());
                msg.text = item["text"].get<std::string>();
                msg.created_at = item["created_at"].get<std::string>();
                messages.push_back(msg);
            }
        }
        // TODO: (1) CurlInitFailed   — curl handle не создался
        // TODO: (2) ConnectionFailed — сервер недоступен
        // TODO: (3) Timeout          — ответ не пришёл за 5 секунд
        // TODO: (4) JsonParseError   — тело ответа не является JSON
        // TODO: (5) InvalidResponse  — нет поля "messages" в ответе
        // TODO: (7) Unauthorized     — неверный пароль пользователя me
        // TODO: (8) NotFound         — пользователь me или peer не найден
        return messages;
    }

    void HttpMessageApi::setUrl(const std::string& url) { url_ = url; }
}