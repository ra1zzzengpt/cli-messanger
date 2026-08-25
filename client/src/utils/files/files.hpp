#ifndef FILES_HPP
#define FILES_HPP

#include <filesystem>
#include <models/message.hpp>
#include <models/chat_info.hpp>
#include <expected>
#include <utils/error/app_error.hpp>

namespace stx {
    std::expected<void, err::Error> dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages,const std::string& peer_nick, const std::uint64_t& peer_id);
}
#endif