#pragma once
#include <optional>
#include <cstdint>
#include <string>
#include "models/user_info.h"

class IMessageApi
{
    public:
    virtual ~IMessageApi() = default;
    virtual bool registerUser(std::uint64_t id, const std::string& nickname) = 0;
    virtual std::optional<UserInfo> getUserInfoById(std::uint64_t id) = 0;
    virtual bool updateNickname(std::uint64_t id, std::string& newNickname) = 0;
};
