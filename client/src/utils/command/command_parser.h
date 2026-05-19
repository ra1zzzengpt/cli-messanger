#pragma once
#include <optional>
#include <string>

namespace utils {
    enum class Command {
        Quit,
        Help,
        Update,
        Dump
    };

    std::optional<Command> parseCommand(const std::string& command); // todo: to expected
};
