#pragma once

#include "chat_info.h"

#include <cstdint>
#include <string>
#include <vector>

struct ServerInfo {
    std::string name;

    std::string host;
    int port = 0;

    std::uint64_t user_id = 0;
    std::string nick;

    std::vector<ChatInfo> chats;
};