#pragma once

#include <filesystem>
#include "models/message.h"
#include "models/chat_info.h"

namespace utils {
    void PrintFromFile(const std::filesystem::path& path);

    void DumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages,const ChatInfo& chat);
}