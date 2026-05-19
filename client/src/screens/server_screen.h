#pragma once

#include "i_screen.h"
#include "app/app_controller.h"

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

    enum class ServerMenu
    {
        MinChoice = 1,
        MaxChoice = 2,
        ChangeHost = 1,
        Exit = 2
    };
}