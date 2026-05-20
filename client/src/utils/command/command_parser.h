#pragma once
#include <expected>
#include <string>

#include "utils/error/app_error.h"

namespace stx {
    enum class Command {
        Quit,
        Help,
        Update,
        Dump
    };

    std::expected<Command,err::AppError> parseCommand(const std::string& command);
};
