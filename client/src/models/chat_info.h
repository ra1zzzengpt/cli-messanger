#pragma once

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

struct ChatInfo {
    std::uint64_t peer_id = 0;
    std::string peer_nick;
    int last_message_id = 0;
};

void to_json(nlohmann::json& j, const ChatInfo& chat);
void from_json(const nlohmann::json& j, ChatInfo& chat);