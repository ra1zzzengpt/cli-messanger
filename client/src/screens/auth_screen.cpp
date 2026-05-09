#include "auth_screen.h"
#include "utils/console/console.h"
#include "main_screen.h"
#include <iostream>

#include "server_screen.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen
{
    AuthScreen::AuthScreen(app::AppController& controller) : controller_(controller) {}

    void AuthScreen::run()
    {
        bool running = true;
        while (running)
        {
            printScreen();
            switch (io::ScanUint32("> "))
            {
                case static_cast<uint32_t>(kAUTH_MENU::kLogin):
                {
                    handleLogin();
                    break;
                }
                case static_cast<uint32_t>(kAUTH_MENU::kRegister):
                {
                    handleRegister();
                    break;
                }
                case static_cast<uint32_t>(kAUTH_MENU::kServerSettings):
                {
                    ServerScreen server_screen(controller_);
                    server_screen.run();
                    break;
                }
                case static_cast<uint32_t>(kAUTH_MENU::kExit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(kAUTH_MENU::kMinChoice)) +
                        " to " + std::to_string(static_cast<int>(kAUTH_MENU::kMaxChoice)), io::COLOR::RED);
                }
            }
        }
    }

    void AuthScreen::printScreen()
    {
        utils::PrintFromFile(paths::AUTH);
        io::print("Your ID from save.json: " + std::to_string(controller_.GetAppConfig().user.id));
    }

    void AuthScreen::handleLogin() const
    {
        const uint64_t id = io::ScanUint64("Enter ID: ");
        const std::string password = io::ScanString("Enter password: ");

        if (controller_.loginUser(id, password))
        {
            io::print("Login successful!",io::COLOR::GREEN);
            if (const std::optional<UserInfo> user_opt = controller_.getNicknameById(id))
            {
                controller_.GetAppConfig().user = *user_opt;
                controller_.GetAppConfig().user.password = password;
                if (controller_.SaveAppConfig())
                {
                    MainScreen main_screen(controller_);
                    main_screen.run();
                } else
                {
                    io::print("[Error]: Failed to save user in config.",io::COLOR::RED);
                    io::WaitForEnter();
                }
            }
        }
        else
        {
            io::print("[Error]: Server is offline or unreachable. Please check settings.",io::COLOR::RED);
            io::WaitForEnter();
        }
    }

    void AuthScreen::handleRegister() const
    {
        const std::string password = io::ScanString("Enter password: ");;

        UserInfo user;
        user.id = controller_.GetAppConfig().user.id;
        user.nickname = controller_.GetAppConfig().user.nickname;
        user.password = password;

        if (controller_.registerUser(user))
        {
            io::print("Registration successful! You can now login.",io::COLOR::GREEN);
            controller_.GetAppConfig().user.password = password;
            io::WaitForEnter();
        }
        else
        {
            io::print("Registration failed.", io::COLOR::RED);
            io::WaitForEnter();
        }
        if (!controller_.SaveAppConfig())
        {
            io::print("[Error]: Failed to save user in config.", io::COLOR::RED);
        }
    }
}
