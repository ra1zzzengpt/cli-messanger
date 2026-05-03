#include "message.h"

void to_json(nlohmann::json& j, const Message& message) {
    j = nlohmann::json{
            {"id", message.id},
            {"from_id", message.from_id},
            {"from_nick", message.from_nick},
            {"to_id", message.to_id},
            {"text", message.text},
            {"created_at", message.created_at}
    };
}

void from_json(const nlohmann::json& j, Message& message) {
    message.id = j.value("id", 0);
    message.from_id = j.value("from_id", 0ULL);
    message.from_nick = j.value("from_nick", "");
    message.to_id = j.value("to_id", 0ULL);
    message.text = j.value("text", "");
    message.created_at = j.value("created_at", "");
}