#pragma once

#include <ftxui/component/component.hpp>
#include <screens/i_fabric.hpp>
#include <app/app_controller.hpp>

namespace screen
{
    class SettingsFabric final : public IFabric
    {
    public:
        explicit SettingsFabric(app::AppController& controller);
        ~SettingsFabric() override = default;
        SettingsFabric(const SettingsFabric&) = delete;
        SettingsFabric& operator=(const SettingsFabric&) = delete;
        SettingsFabric(SettingsFabric&&) = delete;
        SettingsFabric& operator=(SettingsFabric&&) = delete;

        ftxui::Component createScreen(int &tab_index) override;
    private:
        app::AppController& controller_;
        stx::err::Error error_;
        std::string nickname_, password_, url_;
        int inner_tab;
    };
}