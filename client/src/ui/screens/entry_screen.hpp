#include "i_screen.hpp"
#ifndef ENTRY_SCREEN_HPP
#define ENTRY_SCREEN_HPP

#include <ui/screens/i_screen.hpp>
#include <app/app_controller.hpp>

namespace screen
{
    class EntryFabric : public IFabric
    {
    public:
        explicit  EntryFabric(app::AppController& controller);
        ~EntryFabric() override = default;
        EntryFabric(const EntryFabric&) = delete;
        EntryFabric& operator=(const EntryFabric&) = delete;
        EntryFabric(EntryFabric&&) = delete;
        EntryFabric& operator=(EntryFabric&&) = delete;

        ftxui::Component build(int &tab_index, ftxui::ScreenInteractive& screen) override;
    private:
        app::AppController& controller_;
        stx::err::Error error_;
        std::string url_;
    };
}

#endif