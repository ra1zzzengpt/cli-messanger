#include "profile_screen.h"
#include "utils/console/console.h"
#include "utils/files/file_utils.h"

namespace screen {
    ProfileScreen::ProfileScreen(AppConfig& config) : config_(config) {}

    void ProfileScreen::run() {
        bool running = true;
        while (running) {
            printScreen();
            switch (io::ScanUint32("> ")) {
                case static_cast<uint32_t>(kPROFILE_MENU::kChangeNickname): {
                    config_.user.nick = io::ScanString("Enter new nickname: ");
                    utils::SaveConfig(config_);
                    break;
                }
                case static_cast<uint32_t>(kPROFILE_MENU::kExit): {
                    running = false;
                    break;
                }
                default: {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kPROFILE_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kPROFILE_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void ProfileScreen::printScreen() {
        io::print("Profile Screen");
        io::print("Your profile:");
        io::print("Nickname: " + config_.user.nick);
        io::print("Id: " + std::to_string(config_.user.id));
        io::print("1 - Change nickname");
        io::print("2 - Exit");
    }
}
