#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>
#include <string>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

namespace ui::cmp
{
    inline ftxui::Component button(std::string label, std::function<void()> on_click)
    {
        ftxui::ButtonOption option;
        option.label = std::move(label);
        option.on_click = std::move(on_click);
        return ftxui::Button(std::move(option));
    }

    inline ftxui::Component button(
        std::string label,
        std::function<void()> on_click,
        std::function<bool()> enabled,
        const bool bordered = true)
    {
        ftxui::ButtonOption option;
        option.label = std::move(label);
        option.on_click = std::move(on_click);
        option.transform = [enabled = std::move(enabled), bordered](const ftxui::EntryState& state)
        {
            ftxui::Element element = ftxui::text(state.label);
            if (bordered)
            {
                element |= ftxui::border;
            }
            if (!enabled())
            {
                element |= ftxui::dim;
            }
            if (state.focused)
            {
                element |= ftxui::bold;
            }
            return element;
        };
        return ftxui::Button(std::move(option));
    }
}

#endif