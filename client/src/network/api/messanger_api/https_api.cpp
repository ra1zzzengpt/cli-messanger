#include <network/api/messanger_api/https_api.hpp>

#include <curl/curl.h>
#include <network/request_controller/request_controller.hpp>
#include <network/request_controller/http_response.hpp>
#include <utils/error/app_error.hpp>
#include <utils/logger/logs.hpp>
#include <expected>

namespace net::api {

    // GET /ping
    std::expected<std::string, stx::err::Error> MessangerApi::ping() const
    {
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::GET, url_ + "/ping");
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok() || !resp->data.value("ok", false))
            return curl::RequestController::httpErr(*resp);
        return "status: " + resp->data["status"].get<std::string>()
             + " uptime: " + resp->data["uptime"].get<std::string>();
    }

    // POST /users/register
    std::expected<void, stx::err::Error> MessangerApi::registerUser(
        const std::uint64_t id, const std::string& nick, const std::string& password
    ) const
    {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["nick"] = nick;
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::POST, url_ + "/users/register", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
        return {};
    }

    // POST /users/login
    std::expected<void, stx::err::Error> MessangerApi::loginUser(
        const std::uint64_t id, const std::string& password
    ) const
    {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::POST, url_ + "/users/login", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
        return {};
    }

    // POST /users/get
    std::expected<UserInfo, stx::err::Error> MessangerApi::getUsernameById(
        const std::uint64_t id
    ) const
    {
        nlohmann::json body;
        body["id"] = std::to_string(id);
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::POST, url_ + "/users/get", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
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
    std::expected<void, stx::err::Error> MessangerApi::updatePassword(
        const std::uint64_t id, const std::string& currentPassword, const std::string& newPassword
    ) const
    {
        nlohmann::json body;
        body["old_password"] = currentPassword;
        body["password"]     = newPassword;
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(
            curl::RequestMethod::PATCH,
            url_ + "/users/" + std::to_string(id) + "/password",
            body.dump()
        );
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
        return {};
    }

    // PATCH /users/:id/nick
    std::expected<void, stx::err::Error> MessangerApi::updateNickname(
        const std::uint64_t id, const std::string& password, const std::string& newNick
    ) const
    {
        nlohmann::json body;
        body["password"] = password;
        body["nick"]     = newNick;
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(
            curl::RequestMethod::PATCH,
            url_ + "/users/" + std::to_string(id) + "/nick",
            body.dump()
        );
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
        return {};
    }

    // POST /messages/send
    std::expected<void, stx::err::Error> MessangerApi::sendMessage(
        const std::uint64_t fromId, const std::uint64_t toId,
        const std::string& password, const std::string& text
    ) const
    {
        nlohmann::json body;
        body["from_id"]  = std::to_string(fromId);
        body["to_id"]    = std::to_string(toId);
        body["password"] = password;
        body["text"]     = text;
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::POST, url_ + "/messages/send", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
        return {};
    }

    // POST /messages/dump
    std::expected<std::vector<Message>, stx::err::Error> MessangerApi::dumpMessages(
        const std::uint64_t myId, const std::uint64_t peerId, const std::string& password
    ) const
    {
        nlohmann::json body;
        body["me"]       = std::to_string(myId);
        body["peer"]     = std::to_string(peerId);
        body["password"] = password;
        const std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::POST, url_ + "/messages/dump", body.dump());
        if (!resp.has_value())
            return std::unexpected(resp.error());
        if (!resp->is_ok())
            return curl::RequestController::httpErr(*resp);
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

    void MessangerApi::setUrl(const std::string& url) { url_ = url; }
}
