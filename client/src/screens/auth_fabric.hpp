#pragma once
#include <screens/i_fabric.hpp>
#include <app/app_controller.hpp>
#include <ftxui/component/component.hpp>

namespace screen
{
    class AuthFabric : public IFabric
    {
    public:
        explicit AuthFabric(app::AppController& controller);
        ftxui::Component createScreen() override;
    private:
        app::AppController& controller_;
        std::string id_, name_, password_;
    };
}