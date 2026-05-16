#include "chat_info.h"

void to_json(nlohmann::json& json, const ChatInfo& chat)
{
    json = nlohmann::json
    {
        {"peer_id",chat.peer_id},
        {"peer_nick",chat.peer_nick},
    };
}

void from_json(const nlohmann::json& json, ChatInfo& chat)
{
    chat.peer_id = json.value<uint64_t>("peer_id", 0);
    chat.peer_nick = json.value("peer_nick", "");
}