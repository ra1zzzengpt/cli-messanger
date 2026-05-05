#pragma once

#include "chat_info.h"

#include <string>

struct ServerInfo {
    std::string host;
    int port = 0;
};

void to_json(nlohmann::json& j, const ServerInfo& server);
void from_json(const nlohmann::json& j, ServerInfo& server);