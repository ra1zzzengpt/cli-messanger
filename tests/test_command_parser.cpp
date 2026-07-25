#include <gtest/gtest.h>
#include <utils/command/command_parser.hpp>

using stx::parseCommand;
using stx::Command;

TEST(CommandParser, KnownCommands) {
    EXPECT_EQ(parseCommand("/help"),   Command::Help);
    EXPECT_EQ(parseCommand("/quit"),   Command::Quit);
    EXPECT_EQ(parseCommand("/update"), Command::Update);
    EXPECT_EQ(parseCommand("/dump"),   Command::Dump);
}

TEST(CommandParser, UnknownCommandReturnsError) {
    for (const std::string command : {"/help", "/unknown", "/QUIT", "quit", "", "hello"}) {
        const auto result = parseCommand(command);

        ASSERT_FALSE(result.has_value());
        EXPECT_EQ(
            std::get<stx::err::CommandError>(result.error().type),
            stx::err::CommandError::UnknownCommand
        );
        EXPECT_EQ(result.error().message, "unknown command");
    }
}
