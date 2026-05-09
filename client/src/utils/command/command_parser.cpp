#include "command_parser.h"

namespace utils {
    std::optional<COMMAND> ParseCommand(const std::string& command) {
        if (command == "/quit") {
            return COMMAND::QUIT;
        }
        if (command == "/help") {
            return COMMAND::HELP;
        }
        if (command == "/update")
        {
            return COMMAND::UPDATE;
        }
        if (command == "/dump")
        {
            return COMMAND::DUMP;
        }
        return std::nullopt;
    }
}
