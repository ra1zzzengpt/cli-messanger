#pragma once

#include <filesystem>
#include "models/message.h"
#include "models/chat_info.h"

namespace utils {
    void printFromFile(const std::filesystem::path& path); // todo: to expected

    void dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages,const ChatInfo& chat); // todo: to expected
}