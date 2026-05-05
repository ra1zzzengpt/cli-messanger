#include "chat_info.h"

void to_json(nlohmann::json& j, const ChatInfo& chat) {
    j = nlohmann::json{
                {"peer_id",chat.peer_id},
        {"peer_nick",chat.peer_nick},
        {"last_message_id",chat.last_message_id},
    };
}

void from_json(const nlohmann::json& j, ChatInfo& chat) {
    chat.peer_id = j.value("peer_id", 0);
    chat.peer_nick = j.value("peer_nick", "");
    chat.last_message_id = j.value("last_message_id", 0);
}