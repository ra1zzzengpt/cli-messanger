#pragma once

#include <cctype>
#include <functional>
#include <string>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>

namespace ui::cmp
{
    inline ftxui::Component input(
            std::string& content,
            std::string placeholder,
            const bool password = false,
            const bool multiline = false)
    {
        ftxui::InputOption option;
        option.content = &content;
        option.placeholder = std::move(placeholder);
        option.password = password;
        option.multiline = multiline;
        return ftxui::Input(std::move(option));
    }

    inline ftxui::Component numeric_input(std::string& content, std::string placeholder)
    {
        ftxui::Component component = input(content, std::move(placeholder));
        component |= ftxui::CatchEvent([](const ftxui::Event& event)
        {
            return event.is_character()
                && !std::isdigit(static_cast<unsigned char>(event.character().front()));
        });
        return component;
    }
}