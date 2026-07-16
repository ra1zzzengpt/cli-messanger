#include <screens/entry_fabric.hpp>

#include "utils/low_level_utils.hpp"

namespace screen
{
    EntryFabric::EntryFabric(app::AppController &controller) : controller_(controller){ }

    ftxui::Component EntryFabric::build(int& tab_index, ftxui::ScreenInteractive& screen)
    {
        // CLI - MESSANGER
        // separator
        // url input
        // button check and in!

        ftxui::InputOption url_input_opt;
        url_input_opt.content = &url_;
        url_input_opt.multiline = false;
        url_input_opt.placeholder = "url...";

        ftxui::Component url_input = ftxui::Input(url_input_opt);

        ftxui::ButtonOption url_button_opt;
        url_button_opt.label = "next";
        url_button_opt.on_click = [this,&tab_index]
        {
            if (stx::checkNoError(controller_.ping(url_),error_))
            {
                if (stx::checkNoError(controller_.updateUrl(url_),error_))
                {
                    tab_index = to_int(kScreen::kAuth);
                }
            }
        };

        ftxui::Component url_button = ftxui::Button(url_button_opt);

        ftxui::ButtonOption exit_button_opt;
        exit_button_opt.label = "exit";
        exit_button_opt.on_click = [&screen]
        {
            screen.ExitLoopClosure()();
        };

        ftxui::Component exit_button = ftxui::Button(exit_button_opt);

        const ftxui::Component entry_container = ftxui::Container::Vertical({url_input, ftxui::Container::Horizontal({exit_button,url_button})});

        return ftxui::Renderer(entry_container, [this,url_input,exit_button,url_button]
        {
            std::vector<ftxui::Element> elements;
            elements.push_back(ftxui::center(ftxui::text("CLI - MESSANGER")));
            elements.push_back(ftxui::separatorDouble());
            elements.push_back(url_input->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 40) | ftxui::border);
            elements.push_back(ftxui::hbox({exit_button->Render(),url_button->Render()}));
            if (!error_.message.empty())
            {
                elements.push_back(ftxui::text(error_.message) | ftxui::color(ftxui::Color::Red));
            }
            return ftxui::center(ftxui::vbox(elements) | ftxui::border);
        });;
    }
}
