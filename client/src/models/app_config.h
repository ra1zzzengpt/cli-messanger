#pragma once

#include "server_info.h"
#include "user_info.h"

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
