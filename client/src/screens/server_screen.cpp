#include "server_screen.h"

#include "utils/console/console.h"

#include <string>

#include "utils/files/file_utils.h"

namespace screen
{
    ServerScreen::ServerScreen(AppConfig& cfg) : config_(cfg)
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
                    config_.server.host = io::ScanString("Enter new host: ");
                    utils::SaveConfig(config_);
                    break;
                }
                case static_cast<uint32_t>(kSERVER_MENU::kChangePort):
                {
                    config_.server.port = io::ScanUint32("Enter new port: ");
                    utils::SaveConfig(config_);
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
        io::print("Host: " + config_.server.host);
        io::print("Port: " + std::to_string(config_.server.port));
        io::print("1 - Change IP");
        io::print("2 - Change port");
        io::print("3 - Exit");
    }
}
