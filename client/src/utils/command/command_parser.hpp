#pragma once
#include <expected>
#include <string>

#include "utils/error/error_types.hpp"

namespace stx
{
    enum class Command
    {
        Quit,
        Help,
        Update,
        Dump
    };

    std::expected<Command,err::CommandError> parseCommand(const std::string& command);
};
