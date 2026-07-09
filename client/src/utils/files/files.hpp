#pragma once

#include <filesystem>
#include <models/message.hpp>
#include <models/chat_info.hpp>
#include <expected>
#include <utils/error/app_error.hpp>

namespace stx {
    std::expected<void, err::Error> printFromFile(const std::filesystem::path& path);

    std::expected<void, err::Error> dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages,const ChatInfo& chat);
}