#pragma once

#include <cstdint>
#include <string>

struct ChatInfo {
    std::uint64_t peer_id = 0;
    std::string peer_nick;

    int last_message_id = 0;
};