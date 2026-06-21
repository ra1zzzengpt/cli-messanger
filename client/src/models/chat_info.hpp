#pragma once

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

struct ChatInfo
{
    std::uint64_t peer_id = 0;
    std::string peer_nick;
};

void to_json(nlohmann::json& json, const ChatInfo& chat);
void from_json(const nlohmann::json& json, ChatInfo& chat);