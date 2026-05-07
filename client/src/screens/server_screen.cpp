#include "server_screen.h"

#include "utils/console/console.h"

#include <string>

#include "utils/files/file_utils.h"

namespace screen
{
    ServerScreen::ServerScreen(app::AppController& controller) : controller_(controller)
    { }

    void ServerScreen::run()
    {
        bool running = true;
        while (running) {
            printScreen();
            switch (io::ScanUint32("> "))
            {
                case static_cast<uint32_t>(kSERVER_MENU::kChangeIp):
                {
                    controller_.GetAppConfig().server.host = io::ScanString("Enter new host: ");
                    //utils::SaveConfig(config_);
                    break;
                }
                case static_cast<uint32_t>(kSERVER_MENU::kChangePort):
                {
                    controller_.GetAppConfig().server.port = io::ScanUint32("Enter new port: ");
                    //utils::SaveConfig(config_);
                    break;
                }
                case static_cast<uint32_t>(kSERVER_MENU::kExit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kSERVER_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kSERVER_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void ServerScreen::printScreen()
    {
        io::print("Server Screen");
        io::print("Current server:");
        io::print("Host: " + controller_.GetAppConfig().server.host);
        io::print("Port: " + std::to_string(controller_.GetAppConfig().server.port));
        io::print("1 - Change IP");
        io::print("2 - Change port");
        io::print("3 - Exit");
    }
}
