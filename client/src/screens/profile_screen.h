#pragma once

#include "i_screen.h"
#include "app/app_controller.h"

namespace screen
{
    class ProfileScreen final : public IScreen
    {
    public:
        explicit ProfileScreen(app::AppController& controller);
        void run() override;
        void printScreen() override;
    private:
        app::AppController& controller_;
    };
}

enum class kPROFILE_MENU
{
    kMinChoice = 1,
    kMaxChoice = 3,
    kChangeNickname = 1,
    kChangePassword = 2,
    kExit = 3
};