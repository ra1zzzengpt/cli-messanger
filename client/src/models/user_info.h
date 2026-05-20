#pragma once

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>
#include <sodium/utils.h>

struct UserInfo
{
    std::uint64_t id = 0;
    std::string nickname;
    std::string password;

    UserInfo() = default;
    UserInfo(const UserInfo&) = default;
    UserInfo& operator=(const UserInfo&) = default;
    UserInfo(UserInfo&&) = default;
    UserInfo& operator=(UserInfo&&) = default;

    ~UserInfo()
    {
        if (!password.empty())
            sodium_memzero(password.data(), password.size());
    }
};

void to_json(nlohmann::json& json, const UserInfo& user);
void from_json(const nlohmann::json& json, UserInfo& user);
