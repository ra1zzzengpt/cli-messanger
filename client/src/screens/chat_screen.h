#pragma once

#include "i_screen.h"
#include "models/app_config.h"
#include "models/chat_info.h"

namespace screen
{
    class ChatScreen final : public IScreen
    {
    public:
        ChatScreen(AppConfig& cfg, ChatInfo& chat);
        void run() override;
        void printScreen() override;

    private:
        AppConfig& config_;
        ChatInfo& chat_;
    };
}
