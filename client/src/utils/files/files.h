#pragma once

#include <filesystem>
#include "models/message.h"
#include "models/chat_info.h"
#include <expected>
#include <utils/error/app_error.h>

namespace utils {
    std::expected<void, errors::AppError> printFromFile(const std::filesystem::path& path);

    std::expected<void, errors::AppError> dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages,const ChatInfo& chat);
}