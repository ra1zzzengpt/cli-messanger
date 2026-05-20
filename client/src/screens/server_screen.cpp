#include "server_screen.h"

#include "utils/console/console.h"

#include <string>

#include "utils/files/files.h"
#include "utils/files/paths.h"

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
                    io::check(controller_.updateUrl(new_url), "[Error]: Failed to update server URL");
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
                }
            }
        }
    }

    void ServerScreen::printScreen()
    {
        io::check(stx::printFromFile(paths::server), "[Error]: Failed to load server screen");
        io::print("Url: " + controller_.getAppConfig().server.url);
        io::print("Checking server status...");
        if (const std::expected<std::string,stx::err::AppError> ping_result = controller_.ping(); io::check(ping_result))
        {
            io::print(ping_result.value(), io::Color::Green);
        }
    }
}
