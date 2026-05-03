#pragma once

#include <cstdint>
#include <string>

struct Message {
    std::uint32_t id = 0;

    std::uint64_t from_id = 0;
    std::string from_nick;

    std::uint64_t to_id = 0;

    std::string text;
    std::string created_at;
};