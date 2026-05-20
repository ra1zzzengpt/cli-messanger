#pragma once

#include "error_types.h"
#include <variant>

namespace utils::errors
{
    struct AppError
    {
        std::variant<std::monostate,FileError,CryptoError,ConfigError,NetworkError,CommandError,JsonError> type;
        std::string message;
    };
}