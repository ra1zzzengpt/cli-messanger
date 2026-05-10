#include <gtest/gtest.h>
#include "utils/command/command_parser.h"

using utils::parseCommand;
using utils::Command;

TEST(CommandParser, KnownCommands) {
    EXPECT_EQ(parseCommand("/quit"),   Command::Quit);
    EXPECT_EQ(parseCommand("/help"),   Command::Help);
    EXPECT_EQ(parseCommand("/update"), Command::Update);
    EXPECT_EQ(parseCommand("/dump"),   Command::Dump);
}

TEST(CommandParser, UnknownCommandReturnsNullopt) {
    EXPECT_EQ(parseCommand("/unknown"), std::nullopt);
    EXPECT_EQ(parseCommand("/QUIT"),    std::nullopt);
    EXPECT_EQ(parseCommand("quit"),     std::nullopt);
    EXPECT_EQ(parseCommand(""),         std::nullopt);
    EXPECT_EQ(parseCommand("hello"),    std::nullopt);
}
