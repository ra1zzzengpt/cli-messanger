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

TEST(CommandParser, UnknownCommandReturnsNullopt) {
    EXPECT_EQ(parseCommand("/unknown"), stx::err::CommandError::UnknownCommand);
    EXPECT_EQ(parseCommand("/QUIT"),    stx::err::CommandError::UnknownCommand);
    EXPECT_EQ(parseCommand("quit"),     stx::err::CommandError::UnknownCommand);
    EXPECT_EQ(parseCommand(""),         stx::err::CommandError::UnknownCommand);
    EXPECT_EQ(parseCommand("hello"),    stx::err::CommandError::UnknownCommand);
}
