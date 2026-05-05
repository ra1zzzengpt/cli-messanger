#include "app_config.h"

void to_json(nlohmann::json& j, const AppConfig& config) {
    j = nlohmann::json{
        {"server", config.server},
        {"user", config.user},
        {"chats", config.chats},
    };
}

void from_json(const nlohmann::json& j, AppConfig& config) {
    config.server = j.value("server", ServerInfo{});
    config.user = j.value("user", UserInfo{});
    config.chats = j.value("chats", std::vector<ChatInfo>{});
}
