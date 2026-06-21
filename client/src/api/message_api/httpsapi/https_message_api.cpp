#include "https_message_api.hpp"

#include <curl/curl.h>

#include "https_response.hpp"
#include "utils/error/app_error.hpp"
#include "utils/logger/logs.hpp"
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

        std::expected<void, stx::err::Error> ParseResponse(const std::string& buffer, HttpResponse& response) {
            try {
                if (!buffer.empty())
                    response.data = nlohmann::json::parse(buffer);
            } catch (const nlohmann::json::parse_error& e) {
                stx::log::error("JSON parse error: " + std::string(e.what()));
                return std::unexpected(stx::err::Error{
                    stx::err::JsonError::ParsingFailed,
                    "JSON parse error: " + std::string(e.what())
                });
            }
            return {};
        }

        enum class RequestMethod { GET, POST, PATCH };

        std::string methodToString(const RequestMethod method) {
            switch (method) {
                case RequestMethod::GET:   return "GET";
                case RequestMethod::POST:  return "POST";
                case RequestMethod::PATCH: return "PATCH";
            }
            return "UNKNOWN";
        }

        std::expected<HttpResponse, stx::err::Error> request(
            const RequestMethod& method,
            const std::string& url,
            const std::string& json_body = ""
        ) {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle) {
                stx::log::error("curl_easy_init failed for " + url);
                return std::unexpected(stx::err::Error{
                    stx::err::NetworkError::CurlInitFailed, "curl_easy_init failed"
                });
            }

            stx::log::info("sending HTTP " + methodToString(method) + " " + url);

            std::string buffer;
            curl_slist* headers = nullptr;
            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0L); // ONLY FOR TESTS ON LOCAL
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0L); // ONLY FOR TESTS ON LOCAL
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
                if (res == CURLE_OPERATION_TIMEDOUT) {
                    stx::log::warn("request " + methodToString(method) + " " + url + " timed out");
                    return std::unexpected(stx::err::Error{
                        stx::err::NetworkError::Timeout, "timed out"
                    });
                }
                stx::log::error("connection failed for " + methodToString(method) + " " + url +
                    " (curl: " + std::string(curl_easy_strerror(res)) + ")");
                return std::unexpected(stx::err::Error{
                    stx::err::NetworkError::ConnectionFailed, "connection failed"
                });
            }

            curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response.status_code);
            stx::log::info("HTTP " + methodToString(method) + " " + url + " -> " +
                std::to_string(response.status_code));

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
        std::unexpected<stx::err::Error> httpErr(const HttpResponse& response) {
            stx::log::warn("server returned non-OK status " + std::to_string(response.status_code) +
                " (" + statusToString(response.status_code) + ")");
            return std::unexpected(stx::err::Error{
                statusToNetworkError(response.status_code), statusToString(response.status_code)
            });
        }
    }

    // GET /ping
    std::expected<std::string, stx::err::Error> HttpMessageApi::ping() {
        const std::expected<HttpResponse,stx::err::Error> resp = request(RequestMethod::GET, url_ + "/ping");
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok() || !resp->data.value("ok", false))
            return httpErr(*resp);
        return "status: " + resp->data["status"].get<std::string>()
             + " uptime: " + resp->data["uptime"].get<std::string>();
    }

    // POST /users/register
    std::expected<void, stx::err::Error> HttpMessageApi::registerUser(
        const std::uint64_t id, const std::string& nick, const std::string& password
    ) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["nick"] = nick;
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::Error> resp = request(RequestMethod::POST, url_ + "/users/register", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /users/login
    std::expected<void, stx::err::Error> HttpMessageApi::loginUser(
        const std::uint64_t id, const std::string& password
    ) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::Error> resp = request(RequestMethod::POST, url_ + "/users/login", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /users/get
    std::expected<UserInfo, stx::err::Error> HttpMessageApi::getUsernameById(
        const std::uint64_t id
    ) {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        const std::expected<HttpResponse,stx::err::Error> resp = request(RequestMethod::POST, url_ + "/users/get", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        if (!resp->data.contains("user")
            || !resp->data["user"].contains("id")
            || !resp->data["user"].contains("nick")) {
            stx::log::error("getUsernameById: response is missing user fields");
            return std::unexpected(stx::err::Error{
                stx::err::NetworkError::InvalidResponse, "missing user fields"
            });
        }
        return resp->data["user"].get<UserInfo>();
    }

    // PATCH /users/:id/password
    std::expected<void, stx::err::Error> HttpMessageApi::updatePassword(
        const std::uint64_t id, const std::string& currentPassword, const std::string& newPassword
    ) {
        nlohmann::json body;
        body["old_password"] = currentPassword;
        body["password"]     = newPassword;
        const std::expected<HttpResponse,stx::err::Error> resp = request(
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
    std::expected<void, stx::err::Error> HttpMessageApi::updateNickname(
        const std::uint64_t id, const std::string& password, const std::string& newNick
    ) {
        nlohmann::json body;
        body["password"] = password;
        body["nick"]     = newNick;
        const std::expected<HttpResponse,stx::err::Error> resp = request(
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
    std::expected<void, stx::err::Error> HttpMessageApi::sendMessage(
        const std::uint64_t fromId, const std::uint64_t toId,
        const std::string& password, const std::string& text
    ) {
        nlohmann::json body;
        body["from_id"]  = std::to_string(fromId);
        body["to_id"]    = std::to_string(toId);
        body["password"] = password;
        body["text"]     = text;
        const std::expected<HttpResponse,stx::err::Error> resp = request(RequestMethod::POST, url_ + "/messages/send", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        return {};
    }

    // POST /messages/dump
    std::expected<std::vector<Message>, stx::err::Error> HttpMessageApi::dumpMessages(
        const std::uint64_t myId, const std::uint64_t peerId, const std::string& password
    ) {
        nlohmann::json body;
        body["me"]       = std::to_string(myId);
        body["peer"]     = std::to_string(peerId);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::Error> resp = request(RequestMethod::POST, url_ + "/messages/dump", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return httpErr(*resp);
        if (!resp->data.contains("messages")) {
            stx::log::error("dumpMessages: response is missing messages field");
            return std::unexpected(stx::err::Error{
                stx::err::NetworkError::InvalidResponse, "missing messages field"
            });
        }
        std::vector<Message> messages;
        for (const auto& item : resp->data["messages"]) {
            messages.push_back(std::move(item.get<Message>()));
        }
        return messages;
    }

    void HttpMessageApi::setUrl(const std::string& url) { url_ = url; }
}
