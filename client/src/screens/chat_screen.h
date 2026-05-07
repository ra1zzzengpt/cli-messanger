#pragma once

#include "i_screen.h"
#include "app/app_controller.h"
#include "models/chat_info.h"

namespace screen
{
    class ChatScreen final : public IScreen
    {
    public:
        ChatScreen(app::AppController& controller, ChatInfo& chat);
        void run() override;
        void printScreen() override;

    private:
        void displayMessages() const;
        void sendMessage(const std::string& text) const;

        app::AppController& controller_;
        ChatInfo& chat_;
    };
}
