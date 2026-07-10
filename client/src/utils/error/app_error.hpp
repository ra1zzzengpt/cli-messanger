#pragma once

#include <utils/error/error_types.hpp>
#include <variant>
#include <string>

namespace stx::err
{
    // ERROR TYPE, MESSAGE
    struct Error
    {
        std::variant<std::monostate,FileError,CryptoError,ConfigError,NetworkError,CommandError,JsonError,TransformError,PasswordError> type;
        std::string message;
    };
}