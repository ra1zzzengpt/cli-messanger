#pragma once
#include <string>
#include <vector>
#include <optional>

namespace utils::crypto
{
    std::string to_base64(const std::vector<unsigned char>& bytes);
    std::optional<std::vector<unsigned char>> from_base64(const std::string& s);
}