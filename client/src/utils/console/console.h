#pragma once

#include <iostream>
#include <cstdint>

namespace io
{
    struct RGB
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    std::string ansiColor(const RGB& color);

    inline std::string ansiReset()
    {
        return "\033[0m";
    }

    namespace COLOR
    {
        inline constexpr RGB BLACK  {0, 0, 0};
        inline constexpr RGB WHITE  {255, 255, 255};
        inline constexpr RGB RED    {255, 0, 0};
        inline constexpr RGB GREEN  {0, 255, 0};
        inline constexpr RGB BLUE   {0, 120, 255};
        inline constexpr RGB YELLOW {255, 255, 0};
    }

    template <typename T, typename N = std::string_view> void print(const T& value,
            const RGB& color = COLOR::WHITE, const N& separator = "\n")
    {
        std::cout << ansiColor(color) << value << ansiReset() << separator;
    }

    uint32_t ScanUint32(std::string_view prompt);
    uint64_t ScanUint64(std::string_view prompt);
    std::string ScanString(std::string_view prompt);
    void WaitForEnter();
    void ClearConsole();
}
