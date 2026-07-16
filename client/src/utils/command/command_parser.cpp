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
        return std::unexpected(err::Error{err::CommandError::UnknownCommand,"unknown command"});
    }
}
