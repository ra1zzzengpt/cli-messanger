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
                    std::string new_host = io::scanString("Enter new host: ");
                    controller_.getAppConfig().server.host = new_host;
                    controller_.getMessageApi().setHost(new_host);
                    if (controller_.saveAppConfig())
                    {
                        io::print("[Success]: new host changed",io::Color::Green);
                    } else
                    {
                        io::print("[Error]: can't save new host", io::Color::Red);
                    }
                    break;
                }
                case static_cast<uint32_t>(ServerMenu::ChangePort):
                {
                    std::string new_port = io::scanString("Enter new port: ");
                    controller_.getAppConfig().server.host = new_port;
                    controller_.getMessageApi().setPort(new_port);
                    if (controller_.saveAppConfig())
                    {
                        io::print("[Success]: new port changed.",io::Color::Green);
                    } else
                    {
                        io::print("[Error]: can't save new port.", io::Color::Red);
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
                }
            }
        }
    }

    void ServerScreen::printScreen()
    {
        utils::printFromFile(paths::server);
        io::print("Host: " + controller_.getAppConfig().server.host);
        io::print("Port: " + std::to_string(controller_.getAppConfig().server.port));
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
