#include "main_screen.h"

#include "chats_screen.h"
#include "profile_screen.h"
#include "server_screen.h"
#include "utils/console/console.h"

namespace screen
{
    MainScreen::MainScreen(app::AppController& controller) : controller_(controller) {}

    void MainScreen::run()
    {
        bool running = true;
        std::unique_ptr<IScreen> currentScreen;
        controller_.LoadAppConfig();
        while (running)
            {
            printScreen();
            switch (io::ScanUint32("> "))
            {
                case static_cast<uint32_t>(kMAIN_MENU::kChatScreen):
                {
                    //currentScreen = std::make_unique<ChatsScreen>(controller_);
                    //currentScreen->run();
                    break;
                }
                case static_cast<uint32_t>(kMAIN_MENU::kProfileScreen):
                {
                    currentScreen = std::make_unique<ProfileScreen>(controller_);
                    currentScreen->run();
                    break;
                }
                case static_cast<uint32_t>(kMAIN_MENU::kServerScreen):
                {
                    currentScreen = std::make_unique<ServerScreen>(controller_);
                    currentScreen->run();
                    break;
                }
                case static_cast<uint32_t>(kMAIN_MENU::kExit):
                {
                    running = false;
                    break;
                }
                default:
                {
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
