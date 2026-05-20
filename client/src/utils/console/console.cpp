#include "console.h"

#include <algorithm>
#include <limits>

std::string io::ansiColor(const RGB &color)
{
    return "\033[38;2;" +
               std::to_string(color.red) + ";" +
               std::to_string(color.green) + ";" +
               std::to_string(color.blue) + "m";
}

namespace
{
    template<typename T>
    T scanUnsignedInt(const std::string_view prompt)
    {
        std::string input;
        while (true)
        {
            io::print(prompt, io::Color::White, "");
            std::getline(std::cin, input);

            if (input.empty()) continue;

            if (!std::ranges::all_of(input, [](const unsigned char c) { return std::isdigit(c); }))
            {
                io::print("[Error]: Invalid input", io::Color::Red);
                continue;
            }

            try
            {
                const uint64_t temp = std::stoull(input);
                if (temp > std::numeric_limits<T>::max())
                {
                    io::print("[Error]: Value out of range", io::Color::Red);
                    continue;
                }
                return static_cast<T>(temp);
            }
            catch (const std::out_of_range&)
            {
                io::print("[Error]: Value out of range", io::Color::Red);
            }
            catch (const std::invalid_argument&)
            {
                io::print("[Error]: Invalid number format", io::Color::Red);
            }
        }
    }
}

uint32_t io::scanUint32(const std::string_view prompt) { return scanUnsignedInt<uint32_t>(prompt); }
uint64_t io::scanUint64(const std::string_view prompt) { return scanUnsignedInt<uint64_t>(prompt); }

std::string io::scanString(const std::string_view prompt) {
    std::string value;

    while (value.empty()) {
        print(prompt, Color::White, "");
        std::getline(std::cin, value);

        if (value.empty()) {
            print("[Error]: Value cannot be empty", io::Color::Red);
        }
    }

    return value;
}

void io::waitForEnter()
{
    print("Press enter to continue...");
    std::cin.get();
}

void io::clearConsole()
{
    std::cout << "\x1B[3J\x1B[2J\x1B[H";
}