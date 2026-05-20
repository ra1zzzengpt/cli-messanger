#pragma once

#include <filesystem>
#include "models/message.h"
#include "models/chat_info.h"
#include <expected>
#include <utils/error/app_error.h>

namespace stx {
    std::expected<void, err::AppError> printFromFile(const std::filesystem::path& path);

    std::expected<void, err::AppError> dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages,const ChatInfo& chat);
}