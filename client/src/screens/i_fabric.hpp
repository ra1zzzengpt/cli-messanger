#pragma once
#include <ftxui/component/component.hpp>

namespace screen
{
    class IFabric
    {
    public:
        virtual ~IFabric() = default;
        virtual ftxui::Component createScreen() = 0;
    };
}