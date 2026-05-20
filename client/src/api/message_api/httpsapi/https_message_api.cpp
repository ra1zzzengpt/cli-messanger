#include "https_message_api.h"

#include <curl/curl.h>

#include "https_response.h"
#include "utils/error/app_error.h"
#include <expected>

namespace api {
    namespace {
        size_t WriteCallback(const char* ptr, const size_t size, const size_t nmemb, void* userdata) {
            const size_t real_size = size * nmemb;
            if (auto* buffer = static_cast<std::string*>(userdata)) {
                buffer->append(ptr, real_size);
                return real_size;
            }
            return 0;
        }

        std::expected<void, stx::err::AppError> ParseResponse(const std::string& buffer, HttpResponse& response) {
            try {
                if (!buffer.empty())
                    response.data = nlohmann::json::parse(buffer);
            } catch (const nlohmann::json::parse_error& e) {
                return std::unexpected(stx::err::AppError{
                    stx::err::JsonError::ParsingFailed,
                    "JSON parse error: " + std::string(e.what())
                });
            }
            return {};
        }

        enum class RequestMethod { GET, POST, PATCH };

        std::expected<HttpResponse, stx::err::AppError> request(
            const RequestMethod& method,
            const std::string& url,
            const std::string& json_body = ""
        ) {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle)
                return std::unexpected(stx::err::AppError{
                    stx::err::NetworkError::CurlInitFailed, "curl_easy_init failed"
                });

            std::string buffer;
            curl_slist* headers = nullptr;
            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);

            if (method == RequestMethod::POST || method == RequestMethod::PATCH) {
                headers = curl_slist_append(headers, "Content-Type: application/json");
                curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_body.c_str());
                curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
                if (method == RequestMethod::PATCH)
                    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PATCH");
            }

            if (const CURLcode res = curl_easy_perform(handle); res != CURLE_OK) {
                curl_easy_cleanup(handle);
                curl_slist_free_all(headers);
                if (res == CURLE_OPERATION_TIMEDOUT)
                    return std::unexpected(stx::err::AppError{
                        stx::err::NetworkError::Timeout, "timed out"
                    });
                return std::unexpected(stx::err::AppError{
                    stx::err::NetworkError::ConnectionFailed, "connection failed"
                });
            }

            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.status_code);

            if (const auto parsed = ParseResponse(buffer, response); !parsed.has_value()) {
                curl_easy_cleanup(handle);
                curl_slist_free_all(headers);
                return std::unexpected(parsed.error());
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(handle);
            return response;
        }

        stx::err::NetworkError statusToNetworkError(const long code) {
            switch (code) {
                case 400: return stx::err::NetworkError::BadRequest;
                case 401: return stx::err::NetworkError::Unauthorized;
                case 404: return stx::err::NetworkError::NotFound;
                case 409: return stx::err::NetworkError::Conflict;
                default:
                    if (code >= 500) return stx::err::NetworkError::ServerError;
                    return stx::err::NetworkError::UnexpectedStatus;
            }
        }

        std::string statusToString(const long code) {
            switch (code) {
                case 400: return "bad request";
                case 401: return "unauthorized";
                case 404: return "not found";
                case 409: return "conflict";
                default:
                    if (code >= 500) return "server error";
                    return "unknown status";
            }
        }

        // Shortcut: convert non-2xx HttpResponse into std::unexpected
        std::unexpected<stx::err::AppError> httpErr(const HttpResponse& response) {
            return std::unexpected(stx::err::AppError{
                statusToNetworkError(response.status_code), statusToString(response.status_code)
            });
        }
    }

    // GET /ping
    std::expected<std::string, stx::err::AppError> HttpMessageApi::ping() {
        const std::expected<HttpResponse,stx::err::AppError> resp = request(RequestMethod::GET, url_ + "/ping");
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok() || !resp->data.value("ok", false))
            return httpErr(*resp);
        return "status: " + resp->data["status"].get<std::string>()
             + " uptime: " + resp->data["uptime"].get<std::string>();
    }

    // POST /users/register
    std::expected<void, stx::err::AppError> HttpMessageApi::registerUser(
        const std::uint64_t id, const std::string& nick, const std::string& password
    ) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["nick"] = nick;
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(RequestMethod::POST, url_ + "/users/register", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /users/login
    std::expected<void, stx::err::AppError> HttpMessageApi::loginUser(
        const std::uint64_t id, const std::string& password
    ) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(RequestMethod::POST, url_ + "/users/login", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /users/get  — пароль в теле, не в URL
    std::expected<UserInfo, stx::err::AppError> HttpMessageApi::getUsernameById(
        const std::uint64_t id, const std::string& password
    ) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(RequestMethod::POST, url_ + "/users/get", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        if (!resp->data.contains("user")
            || !resp->data["user"].contains("id")
            || !resp->data["user"].contains("nick"))
            return std::unexpected(stx::err::AppError{
                stx::err::NetworkError::InvalidResponse, "missing user fields"
            });
        UserInfo info;
        info.id       = std::stoull(resp->data["user"]["id"].get<std::string>());
        info.nickname = resp->data["user"]["nick"].get<std::string>();
        return info;
    }

    // PATCH /users/:id/password
    std::expected<void, stx::err::AppError> HttpMessageApi::updatePassword(
        const std::uint64_t id, const std::string& currentPassword, const std::string& newPassword
    ) {
        nlohmann::json body;
        body["old_password"] = currentPassword;
        body["password"]     = newPassword;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(
            RequestMethod::PATCH,
            url_ + "/users/" + std::to_string(id) + "/password",
            body.dump()
        );
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // PATCH /users/:id/nick
    std::expected<void, stx::err::AppError> HttpMessageApi::updateNickname(
        const std::uint64_t id, const std::string& password, const std::string& newNick
    ) {
        nlohmann::json body;
        body["password"] = password;
        body["nick"]     = newNick;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(
            RequestMethod::PATCH,
            url_ + "/users/" + std::to_string(id) + "/nick",
            body.dump()
        );
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /messages/send
    std::expected<void, stx::err::AppError> HttpMessageApi::sendMessage(
        const std::uint64_t fromId, const std::uint64_t toId,
        const std::string& password, const std::string& text
    ) {
        nlohmann::json body;
        body["from_id"]  = std::to_string(fromId);
        body["to_id"]    = std::to_string(toId);
        body["password"] = password;
        body["text"]     = text;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(RequestMethod::POST, url_ + "/messages/send", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /messages/dump  — me, peer, пароль в теле, не в URL
    std::expected<std::vector<Message>, stx::err::AppError> HttpMessageApi::dumpMessages(
        const std::uint64_t myId, const std::uint64_t peerId, const std::string& password
    ) {
        nlohmann::json body;
        body["me"]       = std::to_string(myId);
        body["peer"]     = std::to_string(peerId);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::AppError> resp = request(RequestMethod::POST, url_ + "/messages/dump", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        if (!resp->data.contains("messages"))
            return std::unexpected(stx::err::AppError{
                stx::err::NetworkError::InvalidResponse, "missing messages field"
            });
        std::vector<Message> messages;
        for (const auto& item : resp->data["messages"]) {
            Message msg;
            msg.id         = std::stoull(item["id"].get<std::string>());
            msg.from_id    = std::stoull(item["from_id"].get<std::string>());
            msg.to_id      = std::stoull(item["to_id"].get<std::string>());
            msg.text       = item["text"].get<std::string>();
            msg.created_at = item["created_at"].get<std::string>();
            messages.push_back(std::move(msg));
        }
        return messages;
    }

    void HttpMessageApi::setUrl(const std::string& url) { url_ = url; }
}
