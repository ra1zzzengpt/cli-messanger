#include "user_info.h"

void to_json(nlohmann::json& json, const UserInfo& user)
{
    json = nlohmann::json
    {
            {"id", user.id},
            {"nick", user.nickname}
    };
}

void from_json(const nlohmann::json& json, UserInfo& user)
{
    user.id = json.value<std::uint64_t>("id", 0);
    user.nickname = json.value("nick", "");
}