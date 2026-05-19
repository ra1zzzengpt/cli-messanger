#pragma once
#include <expected>
#include <string>

#include "utils/error/app_error.h"

namespace utils {
    enum class Command {
        Quit,
        Help,
        Update,
        Dump
    };

    std::expected<Command,errors::AppError> parseCommand(const std::string& command);
};
