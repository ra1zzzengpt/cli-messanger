#include "app_config.h"

void to_json(nlohmann::json& json, const AppConfig& config)
{
    json = nlohmann::json
    {
        {"server", config.server},
        {"user", config.user},
        {"chats", config.chats},
    };
}

void from_json(const nlohmann::json& json, AppConfig& config)
{
    config.server = json.value("server", ServerInfo{});
    config.user = json.value("user", UserInfo{});
    config.chats = json.value("chats", std::vector<ChatInfo>{});
}
