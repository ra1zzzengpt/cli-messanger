#include <gtest/gtest.h>
#include "utils/command/command_parser.h"

using stx::parseCommand;
using stx::Command;

TEST(CommandParser, KnownCommands) {
    EXPECT_EQ(parseCommand("/quit"),   Command::Quit);
    EXPECT_EQ(parseCommand("/help"),   Command::Help);
    EXPECT_EQ(parseCommand("/update"), Command::Update);
    EXPECT_EQ(parseCommand("/dump"),   Command::Dump);
}

TEST(CommandParser, UnknownCommandReturnsError) {
    const auto unknown = std::unexpected(stx::err::CommandError::UnknownCommand);
    EXPECT_EQ(parseCommand("/unknown"), unknown);
    EXPECT_EQ(parseCommand("/QUIT"),    unknown);
    EXPECT_EQ(parseCommand("quit"),     unknown);
    EXPECT_EQ(parseCommand(""),         unknown);
    EXPECT_EQ(parseCommand("hello"),    unknown);
}
