#include <ui/screens/entry_screen.hpp>
#include <ui/components/button.hpp>
#include <ui/components/input.hpp>

#include "utils/low_level_utils.hpp"

namespace screen
{
    EntryFabric::EntryFabric(app::AppController &controller) : controller_(controller){ }

    ftxui::Component EntryFabric::build(int& tab_index, ftxui::ScreenInteractive& screen)
    {
        ftxui::Component url_input = ui::cmp::input(url_, "url...");
        ftxui::Component url_button = ui::cmp::button("next", [this,&tab_index]
        {
            if (stx::checkNoError(controller_.ping(url_),error_))
            {
                if (stx::checkNoError(controller_.updateUrl(url_),error_))
                {
                    tab_index = to_int(kScreen::kAuth);
                }
            }
        });
        ftxui::Component exit_button = ui::cmp::button("exit", [&screen]
        {
            screen.ExitLoopClosure()();
        });

        const ftxui::Component entry_container = ftxui::Container::Vertical({url_input, ftxui::Container::Horizontal({exit_button,url_button})});

        return ftxui::Renderer(entry_container, [this,url_input,exit_button,url_button]
        {
            return ftxui::center(
                ftxui::vbox({
                    ftxui::text("CLI - MESSANGER") | ftxui::center,
                    ftxui::separatorDouble(),
                    ftxui::text("Choose server: ") | ftxui::center,
                    url_input->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 40) | ftxui::border,
                    ftxui::center(ftxui::hbox({exit_button->Render(), url_button->Render()})),
                    ftxui::text(error_.message) | ftxui::bold | ftxui::color(ftxui::Color::Red) | ftxui::center,
                }) | ftxui::border);
        });
    }
}
