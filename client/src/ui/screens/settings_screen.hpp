#ifndef SETTINGS_SCREEN_HPP
#define SETTINGS_SCREEN_HPP

#include <ftxui/component/component.hpp>
#include <ui/screens/i_screen.hpp>
#include <app/app_controller.hpp>

namespace screen
{
    class SettingsFabric : public IFabric
    {
    public:
        explicit SettingsFabric(app::AppController& controller);
        ~SettingsFabric() override = default;
        SettingsFabric(const SettingsFabric&) = delete;
        SettingsFabric& operator=(const SettingsFabric&) = delete;
        SettingsFabric(SettingsFabric&&) = delete;
        SettingsFabric& operator=(SettingsFabric&&) = delete;

        ftxui::Component build(int &tab_index, ftxui::ScreenInteractive&) override;
    private:
        app::AppController& controller_;
        stx::err::Error error_;
        std::string nickname_, password_, url_;
        int inner_tab_index_;
    };
}

#endif