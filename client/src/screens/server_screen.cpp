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
                    std::string new_host = io::ScanString("Enter new host: ");
                    if (controller_.SaveAppConfig())
                    {
                        controller_.GetAppConfig().server.host = new_host;
                        controller_.GetMessageApi().set_host(new_host);
                        io::print("[Success]: new host changed",io::COLOR::GREEN);
                    } else
                    {
                        io::print("[Error]: can't save new host", io::COLOR::RED);
                    }
                    break;
                }
                case static_cast<uint32_t>(kSERVER_MENU::kChangePort):
                {
                    std::string new_port = io::ScanString("Enter new port: ");
                    if (controller_.SaveAppConfig())
                    {
                        controller_.GetAppConfig().server.host = new_port;
                        controller_.GetMessageApi().set_port(new_port);
                        io::print("[Success]: new port changed.",io::COLOR::GREEN);
                    } else
                    {
                        io::print("[Error]: can't save new port.", io::COLOR::RED);
                    }
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
