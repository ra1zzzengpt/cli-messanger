#include <screens/hello_fabric.h>

namespace screen
{
    HelloFabric::HelloFabric(app::AppController& controller) : controller_(controller) {}

    ftxui::Component HelloFabric::createScreen(int &tab_index, ftxui::ScreenInteractive screen)
    {
        ftxui::Element cli_messanger_label = ftxui::text("CLI-MESSANGER");

        ftxui::Element hello_label = ftxui::text("Hello, " + controller_.getAppConfig().user.nickname + "!");

        ftxui::ButtonOption chats_button_option;
        chats_button_option.label = "chats";
        chats_button_option.on_click = [&]
        {
            // TODO tab_index = CHAT SCREEN
        };

        ftxui::Component chats_button = ftxui::Button(chats_button_option);

        ftxui::ButtonOption settings_button_option;
        settings_button_option.label = "settings";
        settings_button_option.on_click = [&]
        {
            // TODO tab_index = SETTINGS SCREEN
        };

        ftxui::Component settings_button = ftxui::Button(settings_button_option);

        ftxui::ButtonOption exit_button_option;
        exit_button_option.label = "exit";
        exit_button_option.on_click = [&]
        {
            // TODO screen.Exit
        };

        ftxui::Component exit_button = ftxui::Button(exit_button_option);

        ftxui::Component hello_container = ftxui::Container::Vertical({chats_button, settings_button, exit_button});

        ftxui::Component hello_screen = ftxui::Renderer(hello_container,[&]
        {
            ftxui::Element version;
            if (controller_.getCurrentVersion() == controller_.getLatestVersion())
            {
                version = ftxui::text("Latest version: " + controller_.getCurrentVersion());
            } else
            {
                version = ftxui::text("Please update! Current version: " + controller_.getCurrentVersion() + ", Latest version: " + controller_.getLatestVersion() + "!")
                | ftxui::bold | ftxui::hyperlink(/* TODO MAKE MY LINK */);
            }
            return ftxui::vbox(
                cli_messanger_label,
                ftxui::text(""),
                hello_label,
                ftxui::separator(),
                hello_container->Render(),
                version);
        });

        return hello_screen;
    }
}

