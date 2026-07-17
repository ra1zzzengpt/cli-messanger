#pragma once
#include "auth_screen.hpp"
#include "chats_screen.hpp"
#include "entry_screen.hpp"
#include "hello_screen.hpp"
#include "settings_screen.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <thread>

namespace screen
{
    class FabricBuilder
    {
    public:
        explicit FabricBuilder(app::AppController& controller, int starter_index);

        void run();

    private:
        app::AppController& controller_;

        SettingsFabric settings_;
        HelloFabric hello_;
        EntryFabric entry_;
        ChatsFabric chats_;
        AuthFabric auth_;

        void update_chats_loop();
        int tab_index_;
        ftxui::ScreenInteractive screen_;
        std::atomic<bool> running_{true};
        std::thread updater_thread_;
    };
}
