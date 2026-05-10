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

    namespace Color
    {
        inline constexpr RGB Black  {0, 0, 0}; // now not use
        inline constexpr RGB White  {255, 255, 255};
        inline constexpr RGB Red    {255, 0, 0};
        inline constexpr RGB Green  {0, 255, 0};
        inline constexpr RGB Blue   {0, 120, 255}; // now not use
        inline constexpr RGB Yellow {255, 255, 0};
    }

    template <typename T, typename N = std::string_view> void print(const T& value,
            const RGB& color = Color::White, const N& separator = "\n")
    {
        std::cout << ansiColor(color) << value << ansiReset() << separator;
    }

    uint32_t scanUint32(std::string_view prompt);
    uint64_t scanUint64(std::string_view prompt);
    std::string scanString(std::string_view prompt);
    void waitForEnter();
    void clearConsole();
}
