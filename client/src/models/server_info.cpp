#include "server_info.h"

void to_json(nlohmann::json& json, const ServerInfo& server)
{
    json = nlohmann::json
    {
        {"host",server.host},
        {"port",server.port},
    };
}

void from_json(const nlohmann::json& json, ServerInfo& server)
{
    server.host = json.value("host", server.host);
    server.port = json.value("port", server.port);
}