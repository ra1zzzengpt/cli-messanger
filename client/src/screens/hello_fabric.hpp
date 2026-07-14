#pragma once
#include <screens/i_fabric.hpp>
#include <app/app_controller.hpp>
#include <utils/error/app_error.hpp>

#include <ftxui/component/screen_interactive.hpp>

namespace screen
{
    class HelloFabric : public IFabric
    {
    public:
        explicit HelloFabric(app::AppController& controller);
        ~HelloFabric() override = default;
        HelloFabric(const HelloFabric&) = delete;
        HelloFabric& operator=(const HelloFabric&) = delete;
        HelloFabric(HelloFabric&&) = delete;
        HelloFabric& operator=(HelloFabric&&) = delete;

        ftxui::Component createScreen(int &tab_index, ftxui::ScreenInteractive) override;
    private:
        app::AppController& controller_;
        stx::err::Error error_;
    };
}
