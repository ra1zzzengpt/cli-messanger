#include "fake_message_api.h"

bool api::FakeMessageApi::registerUser(std::uint64_t id, const std::string &nick)
{
    return true;
}

std::optional<UserInfo> api::FakeMessageApi::getUsernameById(const std::uint64_t id)
{
    return UserInfo{id,"FakeUser"};
}

bool api::FakeMessageApi::updateNickname(std::uint64_t id, const std::string &newNick)
{
    return true;
}

bool api::FakeMessageApi::sendMessage(std::uint64_t fromId, std::uint64_t toId, const std::string &text)
{
    return true;
}

std::vector<Message> api::FakeMessageApi::fetchMessages(const std::uint64_t myId, const std::uint64_t peerId, const uint64_t sinceMessageId)
{
    Message message;
    message.id = sinceMessageId;
    message.from_id = peerId;
    message.from_nick = "FakeUser";
    message.to_id = myId;
    message.text = "text";
    message.created_at = "[2026-04-05]";
    std::vector<Message> messages;
    messages.push_back(message);
    return messages;
}
