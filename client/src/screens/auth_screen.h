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
}

enum class kAUTH_MENU
{
    kMinChoice = 1,
    kMaxChoice = 4,
    kLogin = 1,
    kRegister = 2,
    kServerSettings = 3,
    kExit = 4
};