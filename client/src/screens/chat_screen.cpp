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
                if (std::optional<utils::Command> command = utils::parseCommand(text); command == utils::Command::Quit) {
                    in_chat = false;
                } else if (command == utils::Command::Help) {
                    utils::printFromFile(paths::help);
                    io::waitForEnter();
                } else if (command == utils::Command::Update) {
                    continue;
                } else if (command == utils::Command::Dump) {
                    utils::dumpToFile(paths::getAssetsBase()/"save"/(chat_.peer_nick + ".txt"),controller_.getMessages(makePeerInfo()),chat_);
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
        if (const auto messages = controller_.getMessages(makePeerInfo()); messages.empty())
        {
            io::print("(No messages yet)", io::Color::Yellow);
        }
        else
        {
            for (const auto& msg : messages)
            {
                std::string time_str = msg.created_at.empty() ? "" : "[" + msg.created_at + "] ";
                std::string prefix = (msg.from_id == controller_.getAppConfig().user.id) ? "[You]: " : "[" + chat_.peer_nick + "]: ";
                io::print(time_str + prefix + msg.text);
            }
        }
    }

    void ChatScreen::sendMessage(const std::string& text) const {
        if (controller_.sendMessage(makePeerInfo(), text))
        {
            io::print("Message sent!", io::Color::Green);
        }
        else
        {
            io::print("Failed to send message", io::Color::Red);
        }
    }

    UserInfo ChatScreen::makePeerInfo() const {
        return UserInfo {chat_.peer_id, chat_.peer_nick};
    }
}
