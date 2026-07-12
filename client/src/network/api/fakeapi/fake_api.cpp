#include <api/message_api/fakeapi/fake_api.hpp>
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

    std::expected<std::string,stx::err::Error> FakeMessageApi::ping()
    {
        return "status: online uptime: 0s";
    }

    std::expected<void,stx::err::Error> FakeMessageApi::registerUser(
        std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<void,stx::err::Error> FakeMessageApi::loginUser(
        std::uint64_t, const std::string&)
    {
        return {};
    }

    std::expected<UserInfo,stx::err::Error> FakeMessageApi::getUsernameById(
        const std::uint64_t id)
    {
        return UserInfo{id, "FakeUser"};
    }

    std::expected<void,stx::err::Error> FakeMessageApi::updatePassword(
        std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<void,stx::err::Error> FakeMessageApi::updateNickname(
        std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<void,stx::err::Error> FakeMessageApi::sendMessage(
        std::uint64_t, std::uint64_t, const std::string&, const std::string&)
    {
        return {};
    }

    std::expected<std::vector<Message>,stx::err::Error> FakeMessageApi::dumpMessages(
        const std::uint64_t myId, const std::uint64_t peerId, const std::string&)
    {
        Message msg;
        msg.id         = 1;
        msg.from_id    = peerId;
        msg.to_id      = myId;
        msg.text       = "Hello! I am a fake user.";
        msg.created_at = now();
        return std::vector{msg};
    }

    void FakeMessageApi::setUrl(const std::string&) {}
}
