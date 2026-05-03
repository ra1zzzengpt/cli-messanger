#pragma once

#include "server_info.h"
#include <vector>
#include "user_info.h"

struct AppConfig
{
    ServerInfo server;
    UserInfo user;
    std::vector<ChatInfo> chats;
};
