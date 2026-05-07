#include "command_parser.h"

namespace utils {
    std::optional<COMMAND> ParseCommand(const std::string& command) {
        if (command == "/quit") {
            return COMMAND::QUIT;
        }
        if (command == "/help") {
            return COMMAND::HELP;
        }
        return std::nullopt;
    }
}
