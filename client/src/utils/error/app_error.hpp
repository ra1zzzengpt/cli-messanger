#pragma once

#include <utils/error/error_types.hpp>
#include <variant>
#include <string>

namespace stx::err
{
    struct Error
    {
        std::variant<std::monostate,FileError,CryptoError,ConfigError,NetworkError,CommandError,JsonError,Base64Error> type;
        std::string message;
    };
}