#include <models/user_info.hpp>
#include <utils/low_level_utils.hpp>

void to_json(nlohmann::json& json, const UserInfo& user)
{
    json = nlohmann::json
    {
            {"id", user.id},
            {"nick", user.nickname},
            {"password", user.password}
    };
}

void from_json(const nlohmann::json& json, UserInfo& user)
{
    user.id = json.contains("id") && json["id"].is_string()
        ? stx::transform<std::uint64_t>(json["id"].get<std::string>()).value_or(0)
        : json.value<std::uint64_t>("id", 0);
    user.nickname = json.value("nick", "");
    user.password = json.value("password", "");
}
