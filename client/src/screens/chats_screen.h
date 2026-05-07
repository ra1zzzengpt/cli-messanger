#pragma once

#include "i_screen.h"
#include "app/app_controller.h"

namespace screen
{
    class ChatsScreen final : public IScreen
    {
    public:
        explicit ChatsScreen(app::AppController& controller);
        void run() override;
        void printScreen() override;

    private:
        void addChat() const;
        app::AppController& controller_;
    };
}

enum class kCHATS_MENU
{
    kMinChoice = 1,
    kMaxChoice = 3,
    kSelectChat = 1,
    kAddChat = 2,
    kExit = 3
};
