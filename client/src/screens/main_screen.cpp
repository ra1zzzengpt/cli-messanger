#include "main_screen.h"

#include "chats_screen.h"
#include "profile_screen.h"
#include "server_screen.h"
#include "utils/console/console.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen
{
    MainScreen::MainScreen(app::AppController& controller) : controller_(controller) {}

    void MainScreen::run()
    {
        bool running = true;
        std::unique_ptr<IScreen> current_screen;
        while (running)
            {
            printScreen();
            switch (io::scanUint32("> "))
            {
                case static_cast<uint32_t>(MainMenu::ChatScreen):
                {
                    current_screen = std::make_unique<ChatsScreen>(controller_);
                    current_screen->run();
                    break;
                }
                case static_cast<uint32_t>(MainMenu::ProfileScreen):
                {
                    current_screen = std::make_unique<ProfileScreen>(controller_);
                    current_screen->run();
                    break;
                }
                case static_cast<uint32_t>(MainMenu::ServerScreen):
                {
                    current_screen = std::make_unique<ServerScreen>(controller_);
                    current_screen->run();
                    break;
                }
                case static_cast<uint32_t>(MainMenu::Exit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(MainMenu::MinChoice)) +
                        " to " + std::to_string(static_cast<int>(MainMenu::MaxChoice)), io::Color::Red);
                }
            }
        }
    }

    void MainScreen::printScreen()
    {
        io::check(stx::printFromFile(paths::main_menu));
    }
}
