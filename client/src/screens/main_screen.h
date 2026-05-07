#pragma once

#include "i_screen.h"
#include "app/app_controller.h"

namespace screen
{
    class MainScreen final : public IScreen
    {
    public:
        explicit MainScreen(app::AppController& controller);
        void run() override;
        void printScreen() override;
    private:
        app::AppController& controller_;
    };
}

enum class kMAIN_MENU
{
    kMinChoice = 1,
    kMaxChoice = 4,
    kChatScreen = 1,
    kProfileScreen = 2,
    kServerScreen = 3,
    kExit = 4
};