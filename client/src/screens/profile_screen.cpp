#include "profile_screen.h"
#include "utils/console/console.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen {
    namespace
    {
        std::string MultiplyStr(const std::string& str,const std::size_t size)
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
            switch (io::ScanUint32("> "))
            {
                case static_cast<uint32_t>(kPROFILE_MENU::kChangeNickname):
                {
                    if (std::string new_nickname = io::ScanString("Enter new nickname: "); controller_.updateNickname(new_nickname))
                    {
                        controller_.GetAppConfig().user.nickname = new_nickname;
                        if (controller_.SaveAppConfig())
                        {
                            io::print("[Success]: nickname was updated",io::COLOR::GREEN);
                        } else
                        {
                            io::print("[Error]: Your nickname will be same.",io::COLOR::RED);
                        }
                    } else
                    {
                        io::print("[Error]: the server did not confirm the request", io::COLOR::RED);
                    }
                    break;
                }
                
                case static_cast<uint32_t>(kPROFILE_MENU::kChangePassword):
                {
                    if (std::string new_password = io::ScanString("Enter new password: "); controller_.updatePassword(new_password))
                    {
                        controller_.GetAppConfig().user.password = new_password;
                        if (controller_.SaveAppConfig())
                        {
                            io::print("[Success]: password was updated", io::COLOR::GREEN);
                        } else
                        {
                            io::print("[Error]: password updated on server but local save failed", io::COLOR::RED);
                        }
                    } else
                    {
                        io::print("[Error]: the server did not confirm the password update", io::COLOR::RED);
                    }
                    break;
                }
                case static_cast<uint32_t>(kPROFILE_MENU::kExit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kPROFILE_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kPROFILE_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void ProfileScreen::printScreen()
    {
        utils::PrintFromFile(paths::PROFILE);
        io::print("Nickname: " + controller_.GetAppConfig().user.nickname);
        io::print("Password: " + MultiplyStr("*",controller_.GetAppConfig().user.password.length()));
        io::print("ID: " + std::to_string(controller_.GetAppConfig().user.id));
    }
}
