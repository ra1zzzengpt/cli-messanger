#pragma once

#include "chat_info.h"

#include <string>

struct ServerInfo {
    std::string host;
    std::string port;
};

void to_json(nlohmann::json& json, const ServerInfo& server);
void from_json(const nlohmann::json& json, ServerInfo& server);