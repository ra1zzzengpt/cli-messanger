#pragma once
#include <screens/i_fabric.hpp>
#include <app/app_controller.hpp>
#include <ftxui/component/component.hpp>
#include <utils/error/app_error.hpp>

namespace screen
{
    class AuthFabric : public IFabric
    {
    public:
        explicit AuthFabric(app::AppController& controller);
        ~AuthFabric() override = default;
        AuthFabric(const AuthFabric&)=delete;
        AuthFabric& operator=(const AuthFabric&)=delete;
        AuthFabric(const AuthFabric&&)=delete;
        AuthFabric& operator=(const AuthFabric&&)=delete;

        ftxui::Component createScreen(int& tab_index) override;
    private:
        app::AppController& controller_;
        std::string id_, name_, password_, password_control_;
        stx::err::Error error_;
        int inner_tab_index_;
    };
}