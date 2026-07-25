#include <utils/command/command_parser.hpp>

namespace stx {
    std::expected<Command,err::Error> parseCommand(const std::string& command) {
        if (command == "/quit") {
            return Command::Quit;
        }
        if (command == "/update")
        {
            return Command::Update;
        }
        if (command == "/dump")
        {
            return Command::Dump;
        }
        if (command == "/help")
        {
            return Command::Help;
        }
        return std::unexpected(err::Error{err::CommandError::UnknownCommand,"unknown command"});
    }
}
