#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "models/user_info.h"
#include "models/message.h"
#include "models/chat_info.h"
#include "models/server_info.h"
#include "models/app_config.h"

// --- UserInfo ---

TEST(UserInfo, SerializeRoundTrip) {
    UserInfo original;
    original.id = 123456789;
    original.nickname = "testuser";
    original.password = "secret";

    nlohmann::json j = original;
    UserInfo restored = j.get<UserInfo>();

    EXPECT_EQ(restored.id,       original.id);
    EXPECT_EQ(restored.nickname, original.nickname);
    EXPECT_EQ(restored.password, original.password);
}

TEST(UserInfo, MissingFieldsFallbackToDefaults) {
    nlohmann::json j = nlohmann::json::object();
    UserInfo user = j.get<UserInfo>();

    EXPECT_EQ(user.id, 0u);
    EXPECT_TRUE(user.nickname.empty());
    EXPECT_TRUE(user.password.empty());
}

// --- Message ---

TEST(Message, SerializeRoundTrip) {
    Message original;
    original.id        = 42;
    original.from_id   = 1;
    original.from_nick = "alice";
    original.to_id     = 2;
    original.text      = "hello";
    original.created_at = "05.10 12:00:00";

    nlohmann::json j = original;
    Message restored = j.get<Message>();

    EXPECT_EQ(restored.id,         original.id);
    EXPECT_EQ(restored.from_id,    original.from_id);
    EXPECT_EQ(restored.from_nick,  original.from_nick);
    EXPECT_EQ(restored.to_id,      original.to_id);
    EXPECT_EQ(restored.text,       original.text);
    EXPECT_EQ(restored.created_at, original.created_at);
}

TEST(Message, EmptyTextSerializes) {
    Message msg;
    msg.text = "";
    nlohmann::json j = msg;
    EXPECT_EQ(j["text"].get<std::string>(), "");
}

// --- ChatInfo ---

TEST(ChatInfo, SerializeRoundTrip) {
    ChatInfo original;
    original.peer_id   = 999;
    original.peer_nick = "bob";

    nlohmann::json j = original;
    ChatInfo restored = j.get<ChatInfo>();

    EXPECT_EQ(restored.peer_id,   original.peer_id);
    EXPECT_EQ(restored.peer_nick, original.peer_nick);
}

// --- AppConfig ---

TEST(AppConfig, SerializeRoundTrip) {
    AppConfig cfg;
    cfg.server.host = "localhost";
    cfg.server.port = 5000;
    cfg.user.id       = 42;
    cfg.user.nickname = "alice";
    cfg.user.password = "pass";

    nlohmann::json j = cfg;
    AppConfig restored = j.get<AppConfig>();

    EXPECT_EQ(restored.server.host,    "localhost");
    EXPECT_EQ(restored.server.port,    5000);
    EXPECT_EQ(restored.user.id,        42u);
    EXPECT_EQ(restored.user.nickname,  "alice");
    EXPECT_EQ(restored.user.password,  "pass");
    EXPECT_TRUE(restored.chats.empty());
}

TEST(AppConfig, ChatsPreservedAfterRoundTrip) {
    AppConfig cfg;
    ChatInfo chat;
    chat.peer_id = 10;
    chat.peer_nick = "charlie";
    cfg.chats.push_back(chat);

    nlohmann::json j = cfg;
    AppConfig restored = j.get<AppConfig>();

    ASSERT_EQ(restored.chats.size(), 1u);
    EXPECT_EQ(restored.chats[0].peer_id,   10u);
    EXPECT_EQ(restored.chats[0].peer_nick, "charlie");
}
