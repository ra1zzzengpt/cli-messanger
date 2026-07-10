#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

namespace screen
{
    class IFabric
    {
    public:
        virtual ~IFabric() = default;
        virtual ftxui::Component createScreen(int& tab_index) = 0;
        virtual ftxui::Component createScreen(int &tab_index, ftxui::ScreenInteractive) = 0;
    };
}