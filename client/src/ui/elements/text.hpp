#pragma once
#include "ftxui/ftxui.hpp"

namespace ui::elm
{
    inline ftxui::Element error_text(const std::string & text)
    {
        return ftxui::text(text) | ftxui::bold | ftxui::color(ftxui::Color::Red);
    }
}
