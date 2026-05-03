#include "user_info.h"

void to_json(nlohmann::json& j, const UserInfo& user) {
    j = nlohmann::json{
            {"id", user.id},
            {"nick", user.nick}
    };
}

void from_json(const nlohmann::json& j, UserInfo& user) {
    user.id = j.value("id", 0ULL);
    user.nick = j.value("nick", "");
}