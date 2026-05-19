#include "command_parser.h"

namespace utils {
    std::optional<Command> parseCommand(const std::string& command) { // todo: to expected
        if (command == "/quit") {
            return Command::Quit;
        }
        if (command == "/help") {
            return Command::Help;
        }
        if (command == "/update")
        {
            return Command::Update;
        }
        if (command == "/dump")
        {
            return Command::Dump;
        }
        return std::nullopt;
    }
}
