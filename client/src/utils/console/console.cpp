#include "console.h"

#include <algorithm>

std::string io::ansiColor(const RGB &color)
{
    return "\033[38;2;" +
               std::to_string(color.red) + ";" +
               std::to_string(color.green) + ";" +
               std::to_string(color.blue) + "m";
}

uint32_t io::scanUint32(const std::string_view prompt) {
    std::string input;
    uint32_t result{};

    bool success = false;
    while (!success)
    {
        print(prompt,Color::White,"");
        std::getline(std::cin, input);

        if (input.empty())
        {
            continue;
        }

        if (const bool valid = std::ranges::all_of(input.begin(), input.end(),
            [](const unsigned char c) {return std::isdigit(c);}); !valid)
        {
            print("[Error]: Invalid input",Color::Red);
            continue;
        }

        try
        {
            const unsigned long temp = std::stoul(input);
            if (temp > UINT32_MAX)
            {
                print("[Error]: Value exceeds maximum uint32_t (4294967295)",Color::Red);
                continue;
            }
            result = static_cast<uint32_t>(temp);
            success = true;
        }
        catch (const std::out_of_range&)
        {
            print("[Error]: Value out of range for uint32_t",Color::Red);
        }
        catch (const std::invalid_argument&)
        {
            print("[Error]: Invalid number format",Color::Red);
        }
    }
    return result;
}

uint64_t io::scanUint64(const std::string_view prompt) {
    std::string input;
    uint64_t result{};

    bool success = false;
    while (!success)
    {
        print(prompt, Color::White, "");
        std::getline(std::cin, input);

        if (input.empty())
        {
            continue;
        }

        if (const bool valid = std::ranges::all_of(input.begin(), input.end(),
            [](const unsigned char c) {return std::isdigit(c);}); !valid)
        {
            print("[Error]: Invalid input", Color::Red);
            continue;
        }

        try
        {
            result = std::stoull(input);
            success = true;
        }
        catch (const std::out_of_range&)
        {
            print("[Error]: Value out of range for uint64_t", Color::Red);
        }
        catch (const std::invalid_argument&)
        {
            print("[Error]: Invalid number format", Color::Red);
        }
    }
    return result;
}

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