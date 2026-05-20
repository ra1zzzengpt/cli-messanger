#pragma once
#include <string>
#include <vector>
#include <expected>

#include "utils/error/app_error.h"

namespace stx::crypto
{
    std::string to_base64(const std::vector<unsigned char>& bytes);
    std::expected<std::vector<unsigned char>,err::AppError> from_base64(const std::string& s);
}
