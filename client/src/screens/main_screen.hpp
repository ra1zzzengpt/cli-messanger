#pragma once

#include <screens/i_screen.hpp>
#include <app/app_controller.hpp>

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

    enum class MainMenu
    {
        MinChoice = 1,
        MaxChoice = 4,
        ChatScreen = 1,
        ProfileScreen = 2,
        ServerScreen = 3,
        Exit = 4
    };
}