#pragma once
#include "i_screen.h"
#include "app/app_controller.h"

namespace screen
{
    class AuthScreen : public IScreen
    {
    public:
        explicit AuthScreen(app::AppController& controller);
        void run() override;
        void printScreen() override;

    private:
        app::AppController& controller_;
        void handleLogin() const;
        void handleRegister() const;
    };

    enum class AuthMenu
    {
        MinChoice = 1,
        MaxChoice = 4,
        Login = 1,
        Register = 2,
        ServerSettings = 3,
        Exit = 4
    };
}