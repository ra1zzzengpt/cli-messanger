#include "files.h"
#include "utils/console/console.h"

#include <fstream>

namespace stx
{
    std::expected<void, err::AppError> printFromFile(const std::filesystem::path& path)
    {
        std::ifstream ifs(path);
        if (!ifs.is_open())
        {
            return std::unexpected(err::AppError{err::FileError::OpenFileFailed,"can't open file with path: " + std::string(path)});
        }
        std::string line;
        while (std::getline(ifs, line))
        {
            io::print(line);
        }
        ifs.close();
        return {};
    }

    std::expected<void, err::AppError> dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages, const ChatInfo& chat)
    {
        std::ofstream ofs(path);
        if (!ofs.is_open())
        {
            return std::unexpected(err::AppError{err::FileError::OpenFileFailed,"can't open file with path: " + std::string(path)});
        }
        for (const Message& message : messages)
        {
            std::string message_line;
            std::string time_str = message.created_at.empty() ? "" : "[" + message.created_at + "] ";
            std::string prefix = (message.from_id != chat.peer_id) ? "[You]: " : "[" + chat.peer_nick + "]: ";
            message_line = time_str + prefix + message.text;
            ofs << message_line << std::endl;
        }
        ofs.close();
        return {};
    }
}
