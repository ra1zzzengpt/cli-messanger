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
            switch (io::ScanUint32("> "))
            {
                case static_cast<uint32_t>(kSERVER_MENU::kChangeIp):
                {
                    std::string new_host = io::ScanString("Enter new host: ");
                    controller_.GetAppConfig().server.host = new_host;
                    controller_.GetMessageApi().set_host(new_host);
                    if (controller_.SaveAppConfig())
                    {
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
                    controller_.GetAppConfig().server.host = new_port;
                    controller_.GetMessageApi().set_port(new_port);
                    if (controller_.SaveAppConfig())
                    {
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
        utils::PrintFromFile(paths::SERVER);
        io::print("Host: " + controller_.GetAppConfig().server.host);
        io::print("Port: " + std::to_string(controller_.GetAppConfig().server.port));
        io::print("Checking server status...");
        if (const std::optional<std::string> host = controller_.Ping(); host.has_value())
        {
            io::print(host.value(), io::COLOR::GREEN);
        } else
        {
            io::print("[Error]: server now is offline", io::COLOR::RED);
        }
    }
}
