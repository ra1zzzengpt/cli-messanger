#pragma once


#include "i_screen.hpp"
#include "app/app_controller.hpp"

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

    enum class ChatsMenu
    {
        MinChoice = 1,
        MaxChoice = 3,
        SelectChat = 1,
        AddChat = 2,
        Exit = 3
    };
}
