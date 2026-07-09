#pragma once

#include <screens/i_screen.hpp>
#include <app/app_controller.hpp>

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
    enum class ProfileMenu
    {
        MinChoice = 1,
        MaxChoice = 3,
        ChangeNickname = 1,
        ChangePassword = 2,
        Exit = 3
    };
}