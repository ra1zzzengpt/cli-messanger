#pragma once
#include <expected>
#include <string>

#include <utils/error/app_error.hpp>

namespace stx
{
    enum class Command
    {
        Quit,
        Update,
        Dump
    };

    std::expected<Command,err::Error> parseCommand(const std::string& command);
};
