#include "server_screen.hpp"

#include "utils/console/console.hpp"

#include <string>

#include "utils/files/files.hpp"
#include "utils/files/paths.hpp"
#include "utils/logger/logs.hpp"

namespace screen
{
    ServerScreen::ServerScreen(app::AppController& controller) : controller_(controller)
    { }

    void ServerScreen::run()
    {
        bool running = true;
        while (running) {
            printScreen();
            switch (io::scanUint32("> "))
            {
                case static_cast<uint32_t>(ServerMenu::ChangeHost):
                {
                    std::string new_url = io::scanString("Enter new url: ");
                    stx::log::info("server URL change requested to " + new_url);
                    if (!io::check(controller_.updateUrl(new_url), "[Error]: Failed to update server URL"))
                    {
                        io::waitForEnter();
                        io::clearConsole();
                    }
                    break;
                }
                case static_cast<uint32_t>(ServerMenu::Exit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(ServerMenu::MinChoice)) +
                        " to " + std::to_string(static_cast<int>(ServerMenu::MaxChoice)), io::Color::Red);
                    io::waitForEnter();
                    io::clearConsole();
                }
            }
        }
    }

    void ServerScreen::printScreen()
    {
        io::clearConsole();
        io::check(stx::printFromFile(paths::server), "[Error]: Failed to load server screen");
        io::print("Url: " + controller_.getAppConfig().server.url);
        io::print("Checking server status...");
        if (const std::expected<std::string,stx::err::Error> ping_result = controller_.ping(); io::check(ping_result))
        {
            io::print(ping_result.value(), io::Color::Green);
        }
    }
}
