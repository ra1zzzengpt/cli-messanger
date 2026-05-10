#include <gtest/gtest.h>
#include "utils/command/command_parser.h"

using utils::ParseCommand;
using utils::COMMAND;

TEST(CommandParser, KnownCommands) {
    EXPECT_EQ(ParseCommand("/quit"),   COMMAND::QUIT);
    EXPECT_EQ(ParseCommand("/help"),   COMMAND::HELP);
    EXPECT_EQ(ParseCommand("/update"), COMMAND::UPDATE);
    EXPECT_EQ(ParseCommand("/dump"),   COMMAND::DUMP);
}

TEST(CommandParser, UnknownCommandReturnsNullopt) {
    EXPECT_EQ(ParseCommand("/unknown"), std::nullopt);
    EXPECT_EQ(ParseCommand("/QUIT"),    std::nullopt);
    EXPECT_EQ(ParseCommand("quit"),     std::nullopt);
    EXPECT_EQ(ParseCommand(""),         std::nullopt);
    EXPECT_EQ(ParseCommand("hello"),    std::nullopt);
}
