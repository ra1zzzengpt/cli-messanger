#pragma once
#include "auth_fabric.hpp"
#include "chats_fabric.hpp"
#include "entry_fabric.hpp"
#include "hello_fabric.hpp"
#include "settings_fabric.hpp"
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
