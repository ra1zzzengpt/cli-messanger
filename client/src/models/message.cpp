#include "message.h"

namespace
{
    uint64_t parse_id(const nlohmann::json& json, const char* key)
    {
        if (!json.contains(key)) return 0;
        const auto& v = json[key];
        if (v.is_string()) return std::stoull(v.get<std::string>());
        if (v.is_number_unsigned()) return v.get<uint64_t>();
        return 0;
    }
}

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
    message.id = parse_id(json, "id");
    message.from_id = parse_id(json, "from_id");
    message.from_nick = json.value("from_nick", "");
    message.to_id = parse_id(json, "to_id");
    message.text = json.value("text", "");
    message.created_at = json.value("created_at", "");
}