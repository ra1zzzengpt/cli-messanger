#include <ui/screens/hello_screen.hpp>
#include <ui/components/button.hpp>
#include <ui/components/input.hpp>

namespace screen
{
    HelloFabric::HelloFabric(app::AppController& controller) : controller_(controller) {}

    ftxui::Component HelloFabric::build(int &tab_index, ftxui::ScreenInteractive& screen)
    {
        ftxui::Element cli_messanger_label = ftxui::text("CLI-MESSANGER");

        ftxui::Component chats_button = ui::cmp::button("chats", [&tab_index]
        {
            tab_index = to_int(kScreen::kChats);
        });
        ftxui::Component settings_button = ui::cmp::button("settings", [&tab_index]
        {
            tab_index = to_int(kScreen::kSettings);
        });
        ftxui::Component exit_button = ui::cmp::button("exit", [&screen]
        {
            screen.ExitLoopClosure()();
        });

        const ftxui::Component hello_container = ftxui::Container::Vertical({chats_button, settings_button, exit_button});

        ftxui::Component hello_renderer = ftxui::Renderer(hello_container,
        [this,cli_messanger_label,chats_button,settings_button,exit_button]
        {
            ftxui::Element version;
            if (controller_.versionControl())
            {
                version = ftxui::text("Latest version: " + controller_.currentVersion());
            } else
            {
                version = ftxui::text("Please update! Current version: " + controller_.currentVersion() + ", Latest version: " + controller_.lastestVersion() + "!")
                | ftxui::bold | ftxui::hyperlink("https://github.com/ra1zzzengpt/cli-messanger");
            }
            return ftxui::center(ftxui::vbox(
                cli_messanger_label,
                ftxui::text(""),
                ftxui::text("Hello, " + controller_.getAppConfig().user.nickname + "!"),
                ftxui::separator(),
                chats_button->Render(),
                settings_button->Render(),
                exit_button->Render(),
                version));
        });

        return hello_renderer;
    }
}
