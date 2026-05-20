#include "https_message_api.h"

#include <curl/curl.h>

#include "https_response.h"
#include "utils/error/app_error.h"
#include <expected>

namespace api {
    namespace {
        size_t WriteCallback (const char* ptr, const size_t size, const size_t nmemb, void* userdata) {
            const size_t real_size = size * nmemb;
            if (auto* buffer = static_cast<std::string*>(userdata)) {
                buffer->append(ptr, real_size);
                return real_size;
            }
            return 0;
        }

        std::expected<void,utils::errors::AppError> ParseResponse (const std::string& buffer, HttpResponse& response) {
            try {
                if (!buffer.empty()) {
                    response.data = nlohmann::json::parse(buffer);
                }
            } catch (const nlohmann::json::parse_error& e) {
                return std::unexpected(utils::errors::AppError{utils::errors::JsonError::ParsingFailed,"JSON parse error: " + std::string(e.what())});
            }
            return {};
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

        enum class RequestMethod {
            GET,
            POST,
            PATCH
        };

        std::expected<HttpResponse,utils::errors::AppError> request (const RequestMethod& method,const std::string& url,const std::string& json_body="") {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle)
            {
                return std::unexpected(utils::errors::AppError{utils::errors::NetworkError::CurlInitFailed,"curl_easy_init failed"});
            }
            std::string buffer;
            curl_slist* headers = nullptr;
            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER,0L);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST,0L);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);
            if (method == RequestMethod::PATCH || method == RequestMethod::POST)
            {
                headers = curl_slist_append(headers, "Content-Type: application/json");
                curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_body.c_str());
                curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
                if (method == RequestMethod::PATCH)
                {
                    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PATCH");
                }
            }
            if (const CURLcode res = curl_easy_perform(handle); res != CURLE_OK)
            {
                curl_easy_cleanup(handle);
                curl_slist_free_all(headers);
                if (res == CURLE_OPERATION_TIMEDOUT)
                {
                    return std::unexpected(utils::errors::AppError{utils::errors::NetworkError::Timeout,"timed out"});
                }
                return std::unexpected(utils::errors::AppError{utils::errors::NetworkError::ConnectionFailed,"connection failed"});
            }
            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.status_code);
            if (std::expected<void,utils::errors::AppError> parsed_response = ParseResponse(buffer, response); !parsed_response.has_value())
            {
                curl_easy_cleanup(handle);
                curl_slist_free_all(headers);
                return std::unexpected(parsed_response.error());
            }
            curl_slist_free_all(headers);
            curl_easy_cleanup(handle);
            return response;
        }
    }

    utils::errors::NetworkError ErrorCodeToNetworkError(const long statusCode) {
        switch (statusCode) {
            case 400: return utils::errors::NetworkError::BadRequest;
            case 401: return utils::errors::NetworkError::Unauthorized;
            case 404: return utils::errors::NetworkError::NotFound;
            case 409: return utils::errors::NetworkError::Conflict;
            default:
                if (statusCode >= 500) return utils::errors::NetworkError::ServerError;
                return utils::errors::NetworkError::UnexpectedStatus;
        }
    }

    std::string NetworkErrorCodeToString(const long statusCode) {
        switch (statusCode) {
            case 400: return "bad request";
            case 401: return "unauthorized";
            case 404: return "not found";
            case 409: return "conflict";
            default:
                if (statusCode >= 500) return "server error";
                return "unknown status";
        }
    }

    std::expected<std::string,utils::errors::AppError> HttpMessageApi::ping()
    {
        const std::expected<HttpResponse,utils::errors::AppError> response = request(RequestMethod::GET,url_ + "/ping");
        if (response.has_value()) {
            if (response.value().is_ok() && response.value().data.value("ok", false))
            {
                std::string result;
                result += "status: " + response.value().data["status"].get<std::string>();
                result += " uptime: " + response.value().data["uptime"].get<std::string>();
                return result;
            }
            return std::unexpected(utils::errors::AppError{ErrorCodeToNetworkError(response.value().status_code),
                NetworkErrorCodeToString(response.value().status_code)});
        }
        return std::unexpected(response.error());
    }

    std::expected<void,utils::errors::AppError> HttpMessageApi::registerUser(const std::uint64_t id, const std::string& nick, const std::string& password) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["nick"] = nick;
        body["password"] = password;
        const std::expected<HttpResponse,utils::errors::AppError> response = request(RequestMethod::POST,url_ + "/users/register", body.dump());
        if (response.has_value()) {
            return {};
        }
        return std::unexpected(response.error());
    }

    std::expected<void,utils::errors::AppError> HttpMessageApi::loginUser(const std::uint64_t id, const std::string& password) { // todo: to expected
        const std::string url = url_ + "/users/login";

        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;

        const std::expected<HttpResponse,utils::errors::AppError> response = request(RequestMethod::POST,url_ + "/users/login", body.dump());
        if (response.has_value()) {
            return {};
        }
        return std::unexpected(response.error());
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
        const HttpResponse resp = request(RequestMethod::PATCH,url_ + "/users/" + std::to_string(id) + "/password", body.dump());
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
