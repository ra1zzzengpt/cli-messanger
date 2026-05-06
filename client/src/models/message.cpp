#include "message.h"

void to_json(nlohmann::json& json, const Message& message) {
    json = nlohmann::json
    {
            {"id", message.id},
            {"from_id", message.from_id},
            {"from_nick", message.from_nick},
            {"to_id", message.to_id},
            {"text", message.text},
            {"created_at", message.created_at}
    };
}

void from_json(const nlohmann::json& json, Message& message) {
    message.id = json.value("id", 0);
    message.from_id = json.value("from_id", 0ULL);
    message.from_nick = json.value("from_nick", "");
    message.to_id = json.value("to_id", 0ULL);
    message.text = json.value("text", "");
    message.created_at = json.value("created_at", "");
}