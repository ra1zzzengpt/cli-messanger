#include "profile_screen.h"
#include "utils/console/console.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen {
    namespace
    {
        std::string multiplyStr(const std::string& str,const std::size_t size)
        {
            std::string result;
            for (std::size_t i = 0; i < size; i++)
            {
                result += str;
            }
            return result;
        }
    }
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
                    if (std::string new_nickname = io::scanString("Enter new nickname: "); controller_.updateNickname(new_nickname))
                    {
                        controller_.getAppConfig().user.nickname = new_nickname;
                        if (controller_.saveAppConfig())
                        {
                            io::print("[Success]: nickname was updated",io::Color::Green);
                        } else
                        {
                            io::print("[Error]: Your nickname will be same.",io::Color::Red);
                        }
                    } else
                    {
                        io::print("[Error]: the server did not confirm the request", io::Color::Red);
                    }
                    break;
                }
                
                case static_cast<uint32_t>(ProfileMenu::ChangePassword):
                {
                    if (std::string new_password = io::scanString("Enter new password: "); controller_.updatePassword(new_password))
                    {
                        controller_.getAppConfig().user.password = new_password;
                        if (controller_.saveAppConfig())
                        {
                            io::print("[Success]: password was updated", io::Color::Green);
                        } else
                        {
                            io::print("[Error]: password updated on server but local save failed", io::Color::Red);
                        }
                    } else
                    {
                        io::print("[Error]: the server did not confirm the password update", io::Color::Red);
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
                }
            }
        }
    }

    void ProfileScreen::printScreen()
    {
        utils::printFromFile(paths::profile);
        io::print("Nickname: " + controller_.getAppConfig().user.nickname);
        io::print("Password: " + multiplyStr("*",controller_.getAppConfig().user.password.length()));
        io::print("ID: " + std::to_string(controller_.getAppConfig().user.id));
    }
}
