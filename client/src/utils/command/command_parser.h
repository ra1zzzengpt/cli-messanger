#pragma once
#include <optional>
#include <string>

namespace utils {
    enum class COMMAND {
        QUIT,
        HELP,
    };

    std::optional<COMMAND> ParseCommand(const std::string& command);
};
