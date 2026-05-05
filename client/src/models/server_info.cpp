#include "server_info.h"

void to_json(nlohmann::json& j, const ServerInfo& server) {
    j = nlohmann::json{
        {"host",server.host},
        {"port",server.port},
    };
}

void from_json(const nlohmann::json& j, ServerInfo& server) {
    server.host = j.value("host", server.host);
    server.port = j.value("port", server.port);
}