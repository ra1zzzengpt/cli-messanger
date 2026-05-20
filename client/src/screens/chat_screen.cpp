#include "chat_screen.h"

#include "utils/command/command_parser.h"
#include "utils/console/console.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen
{
    ChatScreen::ChatScreen(app::AppController& controller, const ChatInfo& chat)
        : controller_(controller), chat_(chat) {}

    void ChatScreen::run()
    {
        bool in_chat = true;
        while (in_chat)
        {
            printScreen();
            displayMessages();

            if (const std::string text = io::scanString("> "); text.starts_with("/")) {
                if (std::expected<stx::Command,stx::err::AppError> command = stx::parseCommand(text); command == stx::Command::Quit) {
                    in_chat = false;
                } else if (command == stx::Command::Help) {
                    io::check(stx::printFromFile(paths::help), "[Error]: Failed to load help");
                    io::waitForEnter();
                } else if (command == stx::Command::Update) {
                    continue;
                } else if (command == stx::Command::Dump) {
                    if (const auto msgs = controller_.getMessages(makePeerInfo()); io::check(msgs, "[Error]: Failed to load messages"))
                        io::check(stx::dumpToFile(paths::getAssetsBase()/"save"/(chat_.peer_nick + ".txt"), msgs.value(), chat_), "[Error]: Failed to dump messages to file");
                } else {
                    io::print("[Error]: unknown command.", io::Color::Red);
                }
            } else {
                sendMessage(text);
            }
        }
    }

    void ChatScreen::printScreen()
    {
        io::print("\n--- Chat with " + chat_.peer_nick + " (ID: " + std::to_string(chat_.peer_id) + ") ---");
    }

    void ChatScreen::displayMessages() const {
        const auto result = controller_.getMessages(makePeerInfo());
        if (!io::check(result, "[Error]: Failed to load messages"))
            return;
        const auto& messages = result.value();
        if (messages.empty())
        {
            io::print("(No messages yet)", io::Color::Yellow);
            return;
        }
        for (const auto& msg : messages)
        {
            std::string time_str = msg.created_at.empty() ? "" : "[" + msg.created_at + "] ";
            std::string prefix = (msg.from_id == controller_.getAppConfig().user.id) ? "[You]: " : "[" + chat_.peer_nick + "]: ";
            io::print(time_str + prefix + msg.text);
        }
    }

    void ChatScreen::sendMessage(const std::string& text) const {
        if (io::check(controller_.sendMessage(makePeerInfo(), text), "[Error]: Failed to send message"))
            io::print("Message sent!", io::Color::Green);
    }

    UserInfo ChatScreen::makePeerInfo() const {
        return UserInfo {chat_.peer_id, chat_.peer_nick};
    }
}
