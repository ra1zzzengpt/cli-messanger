#pragma once

#include "server_info.hpp"
#include "user_info.hpp"

#include <nlohmann/json.hpp>
#include <vector>

struct AppConfig
{
    ServerInfo server;
    UserInfo user;
    std::vector<ChatInfo> chats;
};

void to_json(nlohmann::json& json, const AppConfig& config);
void from_json(const nlohmann::json& json, AppConfig& config);
