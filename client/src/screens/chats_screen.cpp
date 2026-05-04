#include "chats_screen.h"

#include "utils/console/console.h"

#include <string>

namespace screen {
    ChatsScreen::ChatsScreen(AppConfig& cfg) : config_(cfg) {}

    void ChatsScreen::run() {
        bool running = true;
        while (running) {
            printScreen();
            switch (io::SafeScanUint32("> ")) {
                case static_cast<uint32_t>(kCHATS_MENU::kSelectChat): {
                    // todo select chat
                    break;
                }
                case static_cast<uint32_t>(kCHATS_MENU::kAddChat): {
                    // todo add new chat
                    break;
                }
                case static_cast<uint32_t>(kCHATS_MENU::kExit): {
                    running = false;
                    break;
                }
                default: {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kCHATS_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kCHATS_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void ChatsScreen::printScreen() {
        io::print("Chats Screen");
        io::print("1 - Select chat");
        io::print("2 - Add new chat");
        io::print("3 - Exit");
    }
}
