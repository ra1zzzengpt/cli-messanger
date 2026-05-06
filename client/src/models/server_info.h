#pragma once

#include "chat_info.h"

#include <string>

struct ServerInfo {
    std::string host;
    uint32_t port = 0;
};

void to_json(nlohmann::json& json, const ServerInfo& server);
void from_json(const nlohmann::json& json, ServerInfo& server);