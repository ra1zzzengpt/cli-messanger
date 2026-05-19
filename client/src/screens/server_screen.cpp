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
                    controller_.updateUrl(new_url); // todo: need to check by nodiscard
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
        utils::printFromFile(paths::server);
        io::print("Url: " + controller_.getAppConfig().server.url);
        io::print("Checking server status...");
        if (const std::optional<std::string> host = controller_.ping(); host.has_value())
        {
            io::print(host.value(), io::Color::Green);
        } else
        {
            io::print("[Error]: server now is offline", io::Color::Red);
        }
    }
}
