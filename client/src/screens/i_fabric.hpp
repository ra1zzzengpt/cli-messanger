#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

namespace screen
{
    class IFabric
    {
    public:
        virtual ~IFabric() = default;
        virtual ftxui::Component build(int &tab_index, ftxui::ScreenInteractive& screen) = 0;
    };

    enum class kScreen
    {
        kEntry = 0,
        kAuth = 1,
        kHello = 2,
        kSettings = 3,
        kChats = 4
    };

    inline int to_int(const kScreen screen_num)
    {
        switch (screen_num)
        {
            case kScreen::kEntry:
                return 0;
                case kScreen::kAuth:
                return 1;
                case kScreen::kHello:
                return 2;
                case kScreen::kSettings:
                return 3;
                case kScreen::kChats:
                return 4;
        }
        return 0;
    }
}
