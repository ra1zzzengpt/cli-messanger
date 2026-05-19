#include "chats_screen.h"
#include "chat_screen.h"
#include "utils/console/console.h"

#include <algorithm>
#include <string>

#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace
{
    void chatsPrint(const std::vector<ChatInfo>& chats)
    {
        if (chats.empty())
        {
            io::print("No chats yet. Add some!", io::Color::Yellow);
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
            const std::vector<ChatInfo>& chats = controller_.getChats();
            chatsPrint(chats);
            
            switch (io::scanUint32("> "))
            {
                case static_cast<uint32_t>(ChatsMenu::SelectChat):
                {
                    if (chats.empty())
                    {
                        io::print("[Error]: No chats to select", io::Color::Red);
                        break;
                    }

                    if (const uint32_t index = io::scanUint32("Enter chat number: "); index > 0 && index <= chats.size())
                    {
                        ChatScreen chat_view(controller_, chats[index - 1]);
                        chat_view.run();
                    }
                    else
                    {
                        io::print("[Error]: Invalid chat number", io::Color::Red);
                    }
                    break;
                }
                case static_cast<uint32_t>(ChatsMenu::AddChat):
                {
                    addChat();
                    break;
                }
                case static_cast<uint32_t>(ChatsMenu::Exit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(ChatsMenu::MinChoice)) +
                        " to " + std::to_string(static_cast<int>(ChatsMenu::MaxChoice)), io::Color::Red);
                }
            }
        }
    }

    void ChatsScreen::printScreen()
    {
        utils::printFromFile(paths::chats);
    }

    void ChatsScreen::addChat() const {
        const uint64_t peer_id = io::scanUint64("Enter user ID to add: ");

        const std::vector<ChatInfo>& chats = controller_.getChats();
        const auto it = std::ranges::find_if(chats, [peer_id](const ChatInfo& chat) {
            return chat.peer_id == peer_id;
        });

        if (peer_id == controller_.getAppConfig().user.id)
        {
            io::print("[Error]: you can't add chat with you", io::Color::Red);
            return;
        }
        if (it != chats.end())
        {
            io::print("[Error]: Chat with ID " + std::to_string(peer_id) + " already exists", io::Color::Red);
            return;
        }

        const std::optional<UserInfo> user_info = controller_.getNicknameById(peer_id);
        if (!user_info.has_value())
        {
            io::print("[Error]: User with ID " + std::to_string(peer_id) + " not found on server", io::Color::Red);
            return;
        }

        controller_.addChat(ChatInfo{peer_id,user_info->nickname}); // todo: need to check by nodiscard
    }
}
