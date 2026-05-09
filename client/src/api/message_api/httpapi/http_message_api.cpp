#include "http_message_api.h"

#include <curl/curl.h>

#include "http_response.h"

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

        HttpResponse GET (const std::string& url) {
            HttpResponse response;
            CURL* handle = curl_easy_init();
            if (!handle)
            {
                response.error_details = "curl_easy_init failed";
                return response;
            }

            std::string buffer;
            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);

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

            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_body.c_str());
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);

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

            curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
            curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PATCH");
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, json_body.c_str());
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, &buffer);
            curl_easy_setopt(handle, CURLOPT_TIMEOUT, 5L);

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

    std::optional<std::string> HttpMessageApi::ping()
    {
        if (const HttpResponse resp = GET(to_url() + "/ping"); resp.is_ok() && resp.data.value("ok", false))
        {
            std::string result;
            result += "status: " + resp.data["status"].get<std::string>();
            result += " uptime: " + resp.data["uptime"].get<std::string>();
            return result;
        }
        return std::nullopt;
    }

    bool HttpMessageApi::registerUser(const std::uint64_t id, const std::string& nick, const std::string& password) {
        const std::string url = to_url() + "/users/register";

        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["nick"] = nick;
        body["password"] = password;

        const HttpResponse resp = POST(url, body.dump());

        return resp.is_ok() && resp.data.value("ok", false);
    }

    bool HttpMessageApi::loginUser(const std::uint64_t id, const std::string& password) {
        const std::string url = to_url() + "/users/login";

        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;

        const HttpResponse resp = POST(url, body.dump());

        return resp.is_ok() && resp.data.value("ok", false);
    }

    std::optional<UserInfo> HttpMessageApi::getUsernameById(const std::uint64_t id) {
        if (auto resp = GET(to_url() + "/users/" + std::to_string(id)); resp.is_ok() && resp.data.value("ok", false)) {
            UserInfo info;
            info.id = std::stoull(resp.data["user"]["id"].get<std::string>());
            info.nickname = resp.data["user"]["nick"].get<std::string>();
            return info;
        }
        return std::nullopt;
    }

    
    bool HttpMessageApi::updatePassword(const std::uint64_t id, const std::string& newPassword) {
        nlohmann::json body;
        body["password"] = newPassword;
        const HttpResponse resp = PATCH(to_url() + "/users/" + std::to_string(id) + "/password", body.dump());
        return resp.is_ok() && resp.data.value("ok", false);
    }

    bool HttpMessageApi::updateNickname(const std::uint64_t id, const std::string& newNick) {
        nlohmann::json body;
        body["nick"] = newNick;
        const HttpResponse resp = PATCH(to_url() + "/users/" + std::to_string(id) + "/nick", body.dump());
        return resp.is_ok() && resp.data.value("ok", false);
    }

    bool HttpMessageApi::sendMessage(const std::uint64_t fromId, const std::uint64_t toId, const std::string& text) {
        nlohmann::json body;
        body["from_id"] = std::to_string(fromId);
        body["to_id"] = std::to_string(toId);
        body["text"] = text;
        const HttpResponse resp = POST(to_url() + "/messages/send", body.dump());
        return resp.is_ok() && resp.data.value("ok", false);
    }

    
    std::vector<Message> HttpMessageApi::dumpMessages(const std::uint64_t myId, const std::uint64_t peerId) {
        const std::string url = to_url() + "/messages/dump?me=" + std::to_string(myId) +
                          "&peer=" + std::to_string(peerId);
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
        return messages;
    }

    std::vector<Message> HttpMessageApi::fetchMessages(const std::uint64_t myId, const std::uint64_t peerId, const uint64_t sinceMessageId) {
        const std::string url = to_url() + "/messages?me=" + std::to_string(myId) +
                          "&peer=" + std::to_string(peerId) +
                          "&since_id=" + std::to_string(sinceMessageId);
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
        return messages;
    }

    void HttpMessageApi::set_host(const std::string& host) { host_ = host; }
    void HttpMessageApi::set_port(const std::string& port) { port_ = port; }
    std::string HttpMessageApi::to_url() const
    {
        return "http://" + host_ + ":" + port_;
    }
}