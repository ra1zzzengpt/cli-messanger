#include "command_parser.h"

namespace stx {
    std::expected<Command,err::AppError> parseCommand(const std::string& command) {
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
        return std::unexpected(err::AppError{err::CommandError::UnknownCommand, "unknown command"});
    }
}
