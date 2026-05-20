#pragma once

#include "error_types.h"
#include <variant>
#include <string>

namespace stx::err
{
    struct AppError
    {
        std::variant<std::monostate,FileError,CryptoError,ConfigError,NetworkError,CommandError,JsonError,Base64Error> type;
        std::string message;
    };
}