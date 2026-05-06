#include "chat_screen.h"

#include "utils/console/console.h"

namespace screen
{
    ChatScreen::ChatScreen(AppConfig& cfg, ChatInfo& chat) : config_(cfg), chat_(chat)
    { }

    void ChatScreen::run()
    {
        printScreen();

        // todo initialize HttpMessageApi for current server config
        // todo call fetchMessages(config_.user.id, chat_.peer_id, chat_.last_message_id) to preload chat history

        while (true) {
            // todo read message input in infinite mode
            // todo if input starts with "/", call ParseCommand
            // todo else call HttpMessageApi::sendMessage(config_.user.id, chat_.peer_id, input)
        }
    }

    void ChatScreen::printScreen()
    {
        io::print("Chat Screen");
        io::print("Peer: " + chat_.peer_nick);
        io::print("Peer ID: " + std::to_string(chat_.peer_id));
    }
}
