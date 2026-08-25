#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <expected>
#include <string>

#include <utils/error/app_error.hpp>

namespace stx
{
    enum class Command
    {
        Quit,
        Update,
        Dump,
        Help
    };

    std::expected<Command,err::Error> parseCommand(const std::string& command);
};

#endif