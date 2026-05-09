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

            if (const std::string text = io::ScanString("> "); text.starts_with("/")) {
                std::optional<utils::COMMAND> command = utils::ParseCommand(text);
                if (command == utils::COMMAND::QUIT) {
                    in_chat = false;
                }
                if (command == utils::COMMAND::HELP) {
                    utils::PrintFromFile(paths::HELP);
                    io::WaitForEnter();
                }
                if (command == utils::COMMAND::UPDATE)
                {
                    continue;
                }
                if (command == utils::COMMAND::DUMP)
                {
                    UserInfo other_user;
                    other_user.id = chat_.peer_id;
                    other_user.nickname = chat_.peer_nick;
                    utils::DumpToFile(paths::getAssetsBase()/"save"/(chat_.peer_nick + ".txt"),controller_.DumpMessages(other_user),chat_);
                }
                if (command == std::nullopt) {
                    io::print("[Error]: unknown command.", io::COLOR::RED);
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

        if (const auto messages = controller_.GetMessages(other_user, chat_); messages.empty())
        {
            io::print("(No messages yet)", io::COLOR::YELLOW);
        }
        else
        {
            for (const auto& msg : messages)
            {
                std::string time_str = msg.created_at.empty() ? "" : "[" + msg.created_at + "] ";
                std::string prefix = (msg.from_id == controller_.GetAppConfig().user.id) ? "[You]: " : "[" + chat_.peer_nick + "]: ";
                
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

        if (controller_.SendMessage(other_user, text))
        {
            io::print("Message sent!", io::COLOR::GREEN);
        }
        else
        {
            io::print("Failed to send message", io::COLOR::RED);
        }
    }
}
