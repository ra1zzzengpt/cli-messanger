#pragma once

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

struct Message {
    std::uint64_t id = 0;

    std::uint64_t from_id = 0;
    std::string from_nick;

    std::uint64_t to_id = 0;

    std::string text;
    std::string created_at;
};

void to_json(nlohmann::json& json, const Message& message);
void from_json(const nlohmann::json& json, Message& message);