#include "chats_screen.h"
#include "chat_screen.h"
#include "utils/console/console.h"

#include <algorithm>
#include <string>

#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace
{
    void chats_print(const std::vector<ChatInfo>& chats)
    {
        if (chats.empty())
        {
            io::print("No chats yet. Add some!", io::COLOR::YELLOW);
            return;
        }

        for (std::size_t i = 0; i != chats.size(); ++i)
        {
            io::print("[" + std::to_string(i + 1) + "] - "
                + chats[i].peer_nick + " (ID: " + std::to_string(chats[i].peer_id) + ")");
        }
    }
}

namespace screen
{
    ChatsScreen::ChatsScreen(app::AppController& controller) : controller_(controller) {}

    void ChatsScreen::run()
    {
        bool running = true;
        while (running) {
            printScreen();
            auto& chats = controller_.GetChats();
            chats_print(chats);
            
            switch (io::ScanUint32("> "))
            {
                case static_cast<uint32_t>(kCHATS_MENU::kSelectChat):
                {
                    if (chats.empty())
                    {
                        io::print("[Error]: No chats to select", io::COLOR::RED);
                        break;
                    }

                    if (uint32_t index = io::ScanUint32("Enter chat number: "); index > 0 && index <= chats.size())
                    {
                        ChatScreen chat_view(controller_, chats[index - 1]);
                        chat_view.run();
                    }
                    else
                    {
                        io::print("[Error]: Invalid chat number", io::COLOR::RED);
                    }
                    break;
                }
                case static_cast<uint32_t>(kCHATS_MENU::kAddChat):
                {
                    addChat();
                    break;
                }
                case static_cast<uint32_t>(kCHATS_MENU::kExit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kCHATS_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kCHATS_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void ChatsScreen::printScreen()
    {
        utils::PrintFromFile(paths::CHATS);
    }

    void ChatsScreen::addChat() const {
        const uint64_t peer_id = io::ScanUint64("Enter user ID to add: ");

        auto& chats = controller_.GetChats();
        const auto it = std::ranges::find_if(chats, [peer_id](const ChatInfo& chat) {
            return chat.peer_id == peer_id;
        });

        if (peer_id == controller_.GetAppConfig().user.id)
        {
            io::print("[Error]: you can't add chat with you", io::COLOR::RED);
            return;
        }
        if (it != chats.end())
        {
            io::print("[Error]: Chat with ID " + std::to_string(peer_id) + " already exists", io::COLOR::RED);
            return;
        }

        const auto user_info = controller_.getNicknameById(peer_id);
        if (!user_info.has_value())
        {
            io::print("[Error]: User with ID " + std::to_string(peer_id) + " not found on server", io::COLOR::RED);
            return;
        }

        ChatInfo new_chat;
        new_chat.peer_id = peer_id;
        new_chat.peer_nick = user_info->nickname;
        new_chat.last_message_id = 0;

        chats.push_back(new_chat);
        if (controller_.SaveAppConfig())
        {
            io::print("[Success]: Added chat with " + new_chat.peer_nick, io::COLOR::GREEN);
        }
        else
        {
            io::print("[Error]: Failed to save chat to configuration", io::COLOR::RED);
        }
    }
}
