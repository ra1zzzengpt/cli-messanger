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
        url_input_opt.placeholder = "url for cli-messanger server...";

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

        const ftxui::Component entry_container = ftxui::Container::Vertical({url_input, url_button, exit_button});

        return ftxui::Renderer(entry_container, [this,entry_container]
        {
            std::vector<ftxui::Element> elements;
            elements.push_back(ftxui::text("CLI - MESSANGER"));
            elements.push_back(ftxui::separator());
            elements.push_back(entry_container->Render());
            if (!error_.message.empty())
            {
                elements.push_back(ftxui::text(error_.message));
            }
            return ftxui::center(ftxui::vbox(elements) | ftxui::border);
        });;
    }
}
