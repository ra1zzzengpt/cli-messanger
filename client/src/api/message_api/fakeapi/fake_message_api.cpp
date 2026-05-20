#include "fake_message_api.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace {
    std::string now() {
        const auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&t), "%m.%d %H:%M:%S");
        return ss.str();
    }
}

namespace api {

    std::expected<std::string,stx::err::AppError> FakeMessageApi::ping()
    {
        return "status: online uptime: 0s";
    }

    std::expected<void,stx::err::AppError> FakeMessageApi::registerUser(
        std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<void,stx::err::AppError> FakeMessageApi::loginUser(
        std::uint64_t, const std::string&)
    {
        return {};
    }

    std::expected<UserInfo,stx::err::AppError> FakeMessageApi::getUsernameById(
        const std::uint64_t id, const std::string&)
    {
        return UserInfo{id, "FakeUser"};
    }

    std::expected<void,stx::err::AppError> FakeMessageApi::updatePassword(
        std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<void,stx::err::AppError> FakeMessageApi::updateNickname(
        std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<void,stx::err::AppError> FakeMessageApi::sendMessage(
        std::uint64_t, std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<std::vector<Message>,stx::err::AppError> FakeMessageApi::dumpMessages(
        const std::uint64_t myId, const std::uint64_t peerId, const std::string&)
    {
        Message msg;
        msg.id         = 1;
        msg.from_id    = peerId;
        msg.to_id      = myId;
        msg.text       = "Hello! I am a fake user.";
        msg.created_at = now();
        return std::vector<Message>{msg};
    }

    void FakeMessageApi::setUrl(const std::string&) {}
}
