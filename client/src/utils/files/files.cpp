#include <utils/files/files.hpp>
#include <utils/logger/logs.hpp>

#include <fstream>

namespace stx
{
    std::expected<void, err::Error> dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages, const std::string& peer_nick, const uint64_t& peer_id)
    {
        std::ofstream ofs(path);
        if (!std::filesystem::exists(path))
        {
            std::error_code error_code;
            std::filesystem::create_directories(path.parent_path(), error_code);
            if (error_code)
            {
                return std::unexpected(err::Error{
            err::FileError::OpenFileFailed,
            "failed to create log directory '" + path.parent_path().string() + "': " + error_code.message()
        });
            }
        }
        if (!ofs.is_open())
        {
            log::error("can't open file for writing: " + std::string(path));
            return std::unexpected(err::Error{err::FileError::OpenFileFailed,"can't open file with path: " + std::string(path)});
        }
        for (const Message& message : messages)
        {
            std::string message_line;
            std::string time_str = message.created_at.empty() ? "" : "[" + message.created_at + "] ";
            std::string prefix = (message.from_id != peer_id) ? "[You]: " : "[" + peer_nick + "]: ";
            message_line = time_str + prefix + message.text;
            ofs << message_line << std::endl;
        }
        ofs.close();
        log::info("dumped " + std::to_string(messages.size()) + " messages to " + std::string(path));
        return {};
    }
}
