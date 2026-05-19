#include "server_info.h"

void to_json(nlohmann::json& json, const ServerInfo& server)
{
    json = nlohmann::json
    {
        {"url",server.url},
    };
}

void from_json(const nlohmann::json& json, ServerInfo& server)
{
    server.url = json.value("url", server.url);
}