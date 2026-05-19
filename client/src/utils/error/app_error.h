#pragma once

#include "error_types.h"

namespace utils::errors
{
    struct AppError
    {
        std::variant<std::monostate,FileError,CryptoError,ConfigError,NetError,CommandError> type;
        std::string message;
    };
}