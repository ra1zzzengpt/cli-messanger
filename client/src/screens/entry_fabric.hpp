#pragma once
#include <screens/i_fabric.hpp>
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
