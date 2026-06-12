#include "profile_screen.h"
#include "utils/console/console.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"
#include "utils/logger/logs.h"

namespace screen {
    ProfileScreen::ProfileScreen(app::AppController& controller) : controller_(controller)
    { }

    void ProfileScreen::run()
    {
        bool running = true;
        while (running)
        {
            printScreen();
            switch (io::scanUint32("> "))
            {
                case static_cast<uint32_t>(ProfileMenu::ChangeNickname):
                {
                    const std::string new_nickname = io::scanString("Enter new nickname: ");
                    stx::log::info("nickname change requested");
                    if (io::check(controller_.updateNickname(new_nickname), "[Error]: Failed to update nickname on server"))
                    {
                        stx::log::info("nickname updated on server");
                        if (!io::check(controller_.updateConfigNickname(new_nickname), "[Error]: Failed to save nickname to config"))
                        {
                            io::waitForEnter();
                            io::clearConsole();
                        }
                        io::waitForEnter();
                        io::clearConsole();
                    }
                    break;
                }

                case static_cast<uint32_t>(ProfileMenu::ChangePassword):
                {
                    const std::string new_password = io::scanString("Enter new password: ");
                    stx::log::info("password change requested");
                    if (io::check(controller_.updatePassword(new_password), "[Error]: Failed to update password on server"))
                    {
                        stx::log::info("password updated on server");
                        if (!io::check(controller_.updateConfigPassword(new_password), "[Error]: Failed to save password to config"))
                        {
                            io::waitForEnter();
                            io::clearConsole();
                        }
                    } else
                    {
                        io::waitForEnter();
                        io::clearConsole();
                    }
                    break;
                }
                case static_cast<uint32_t>(ProfileMenu::Exit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(ProfileMenu::MinChoice)) +
                        " to " + std::to_string(static_cast<int>(ProfileMenu::MaxChoice)), io::Color::Red);
                    io::waitForEnter();
                    io::clearConsole();
                }
            }
        }
    }

    void ProfileScreen::printScreen()
    {
        io::clearConsole();
        io::check(stx::printFromFile(paths::profile), "[Error]: Failed to load profile screen");
        io::print("Nickname: " + controller_.getAppConfig().user.nickname);
        io::print("Password: " + std::string(controller_.getAppConfig().user.password.length(),'*'));
        io::print("ID: " + std::to_string(controller_.getAppConfig().user.id));
    }
}
