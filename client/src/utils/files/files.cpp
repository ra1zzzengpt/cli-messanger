#include "files.h"
#include "utils/console/console.h"

#include <fstream>

namespace utils
{
    void printFromFile(const std::filesystem::path& path)
    {
        std::ifstream ifs(path);
        if (!ifs.is_open())
        {
            io::print("[Error]: can't open " + path.string(),io::Color::Red); // todo: REMOVE IO INCLUDE
            return;
        }
        std::string line;
        while (std::getline(ifs, line))
        {
            io::print(line);
        }
        ifs.close();
    }

    void dumpToFile(const std::filesystem::path& path, const std::vector<Message>& messages, const ChatInfo& chat)
    {
        std::ofstream ofs(path);
        if (!ofs.is_open())
        {
            io::print("[Error]: can't open " + path.string(),io::Color::Red); // todo: REMOVE IO INCLUDE
            return;
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
    }
}
