#pragma once

#include "i_screen.h"
#include "app/app_controller.h"
#include "models/app_config.h"

namespace screen
{
    class ServerScreen final : public IScreen
    {
    public:
        explicit ServerScreen(app::AppController& controller);
        void run() override;
        void printScreen() override;

    private:
        app::AppController& controller_;
    };
}

enum class kSERVER_MENU
{
    kMinChoice = 1,
    kMaxChoice = 3,
    kChangeIp = 1,
    kChangePort = 2,
    kExit = 3
};
