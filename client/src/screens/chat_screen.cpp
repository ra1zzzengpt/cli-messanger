#include "chat_screen.h"

#include "utils/command/command_parser.h"
#include "utils/console/console.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen
{
    ChatScreen::ChatScreen(app::AppController& controller, ChatInfo& chat) 
        : controller_(controller), chat_(chat) {}

    void ChatScreen::run()
    {
        bool in_chat = true;
        while (in_chat)
        {
            printScreen();
            displayMessages();

            if (const std::string text = io::scanString("> "); text.starts_with("/")) {
                std::optional<utils::Command> command = utils::parseCommand(text);
                if (command == utils::Command::Quit) {
                    in_chat = false;
                }
                if (command == utils::Command::Help) {
                    utils::printFromFile(paths::help);
                    io::waitForEnter();
                }
                if (command == utils::Command::Update)
                {
                    continue;
                }
                if (command == utils::Command::Dump)
                {
                    UserInfo other_user;
                    other_user.id = chat_.peer_id;
                    other_user.nickname = chat_.peer_nick;
                    utils::dumpToFile(paths::getAssetsBase()/"save"/(chat_.peer_nick + ".txt"),controller_.dumpMessages(other_user),chat_);
                }
                if (command == std::nullopt) {
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
        UserInfo other_user;
        other_user.id = chat_.peer_id;
        other_user.nickname = chat_.peer_nick;

        if (const auto messages = controller_.getMessages(other_user, chat_); messages.empty())
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
                
                if (msg.id > chat_.last_message_id)
                {
                    chat_.last_message_id = static_cast<int>(msg.id);
                }
            }
        }
    }

    void ChatScreen::sendMessage(const std::string& text) const {
        
        UserInfo other_user;
        other_user.id = chat_.peer_id;
        other_user.nickname = chat_.peer_nick;

        if (controller_.sendMessage(other_user, text))
        {
            io::print("Message sent!", io::Color::Green);
        }
        else
        {
            io::print("Failed to send message", io::Color::Red);
        }
    }
}
