#pragma once

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

struct UserInfo
{
    std::uint64_t id = 0;
    std::string nickname;
    std::string password;
};

void to_json(nlohmann::json& json, const UserInfo& user);
void from_json(const nlohmann::json& json, UserInfo& user);
