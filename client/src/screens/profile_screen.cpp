#include "profile_screen.h"
#include "utils/console/console.h"
#include "utils/files/file_utils.h"

namespace screen
{
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
                        if (!controller_.SaveAppConfig())
                        {
                            io::print("[Error]: Your nickname will be same",io::COLOR::RED);
                        }
                    } else
                    {
                        io::print("[Error]: the server did not confirm the request", io::COLOR::RED);
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
        io::print("Profile Screen");
        io::print("Your profile:");
        io::print("Nickname: " + controller_.GetAppConfig().user.nickname);
        io::print("Id: " + std::to_string(controller_.GetAppConfig().user.id));
        io::print("1 - Change nickname");
        io::print("2 - Exit");
    }
}
