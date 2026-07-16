#include "fabric_builder.hpp"
#include <chrono>
#include <thread>

namespace screen
{
    FabricBuilder::FabricBuilder(app::AppController& controller, const int starter_index)
        : controller_(controller),
            settings_(controller),
          hello_(controller),
          entry_(controller),
          chats_(controller),
          auth_(controller),
          tab_index_(starter_index),
          screen_(ftxui::ScreenInteractive::Fullscreen())
    {}

    void FabricBuilder::run()
    {
        running_ = true;
        updater_thread_ = std::thread(&FabricBuilder::update_chats_loop, this);

        ftxui::Component hello_screen = hello_.build(tab_index_, screen_);
        ftxui::Component entry_screen = entry_.build(tab_index_, screen_);
        ftxui::Component auth_screen  = auth_.build(tab_index_, screen_);
        ftxui::Component chats_screen = chats_.build(tab_index_, screen_);
        ftxui::Component settings_screen = settings_.build(tab_index_, screen_);

        const ftxui::Component tabs = ftxui::Container::Tab({
            entry_screen,
            auth_screen,
            hello_screen,
            settings_screen,
            chats_screen
        }, &tab_index_);

        screen_.Loop(tabs);
        running_ = false;
        if (updater_thread_.joinable())
            updater_thread_.join();
    }

    void FabricBuilder::update_chats_loop()
    {
        constexpr std::chrono::seconds FAILURE_INTERVAL = std::chrono::seconds(5);
        constexpr std::chrono::seconds SUCCESS_INTERVAL = std::chrono::seconds(3);

        while (running_)
        {
            bool success = false;
            if (!controller_.getChats().empty())
            {
                success = chats_.messages_update();
                if (success)
                {
                    screen_.PostEvent(ftxui::Event::Custom);
                }
            }
            auto interval = success ? SUCCESS_INTERVAL : FAILURE_INTERVAL;
            std::this_thread::sleep_for(interval);
        }
    }
}