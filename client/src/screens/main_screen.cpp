#include "main_screen.h"
#include "utils/console/console.h"

namespace screen {
    MainScreen::MainScreen(AppConfig& cfg) : config_(cfg) {}

    void MainScreen::run() {
        bool running = true;
        while (running) {
            printScreen();
            switch (io::SafeScanUint32("> ")) {
                case static_cast<uint32_t>(kMAIN_MENU::kChatScreen): {
                    // todo chats screen
                    break;
                }
                case static_cast<uint32_t>(kMAIN_MENU::kProfileScreen): {
                    // todo profile screen
                    break;
                }
                case static_cast<uint32_t>(kMAIN_MENU::kServerScreen): {
                    // todo server screen
                    break;
                }
                case static_cast<uint32_t>(kMAIN_MENU::kExit): {
                    running = false;
                    break;
                }
                default: {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kMAIN_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kMAIN_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void MainScreen::printScreen() {
        io::print("Main Screen");
        io::print("1 - Chats");
        io::print("2 - Profile");
        io::print("3 - Server");
        io::print("4 - Exit");
    }
}
