#ifndef SERVER_INFO_HPP
#define SERVER_INFO_HPP

#include <models/chat_info.hpp>

#include <string>

struct ServerInfo
{
    std::string url;
};

void to_json(nlohmann::json& json, const ServerInfo& server);
void from_json(const nlohmann::json& json, ServerInfo& server);

#endif