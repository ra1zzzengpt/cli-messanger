// #include "fake_message_api.h"
// #include <chrono>
// #include <iomanip>
// #include <sstream>
//
// namespace {
//     std::string getCurrentTimestamp() {
//         const auto now = std::chrono::system_clock::now();
//         const auto time = std::chrono::system_clock::to_time_t(now);
//         std::stringstream ss;
//         ss << std::put_time(std::localtime(&time), "%H:%M:%S");
//         return ss.str();
//     }
// }
//
// bool api::FakeMessageApi::registerUser(std::uint64_t id, const std::string &nick)
// {
//     return true;
// }
//
// std::optional<UserInfo> api::FakeMessageApi::getUsernameById(const std::uint64_t id)
// {
//     return UserInfo{id, "FakeUser"};
// }
//
// bool api::FakeMessageApi::updateNickname(std::uint64_t id, const std::string &newNick)
// {
//     return true;
// }
//
// bool api::FakeMessageApi::sendMessage(std::uint64_t fromId, std::uint64_t toId, const std::string &text)
// {
//     return true;
// }
//
// std::vector<Message> api::FakeMessageApi::fetchMessages(const std::uint64_t myId, const std::uint64_t peerId, const uint64_t sinceMessageId)
// {
//     std::vector<Message> messages;
//
//     if (sinceMessageId > 0) {
//         Message message;
//         message.id = static_cast<uint32_t>(sinceMessageId + 1);
//         message.from_id = peerId;
//         message.from_nick = "FakeUser";
//         message.to_id = myId;
//         message.text = "Response to your message (ID: " + std::to_string(sinceMessageId) + ")";
//         message.created_at = getCurrentTimestamp();
//         messages.push_back(message);
//     } else {
//         Message welcome;
//         welcome.id = 1;
//         welcome.from_id = peerId;
//         welcome.from_nick = "FakeUser";
//         welcome.to_id = myId;
//         welcome.text = "Hello! I am a fake user.";
//         welcome.created_at = getCurrentTimestamp();
//         messages.push_back(welcome);
//     }
//
//     return messages;
// }
