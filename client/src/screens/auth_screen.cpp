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
            switch (io::scanUint32("> "))
            {
                case static_cast<uint32_t>(AuthMenu::Login):
                {
                    handleLogin();
                    break;
                }
                case static_cast<uint32_t>(AuthMenu::Register):
                {
                    handleRegister();
                    break;
                }
                case static_cast<uint32_t>(AuthMenu::ServerSettings):
                {
                    ServerScreen server_screen(controller_);
                    server_screen.run();
                    break;
                }
                case static_cast<uint32_t>(AuthMenu::Exit):
                {
                    running = false;
                    break;
                }
                default:
                {
                    io::print("[Error]: Enter value from " + std::to_string(static_cast<int>(AuthMenu::MinChoice)) +
                        " to " + std::to_string(static_cast<int>(AuthMenu::MaxChoice)), io::Color::Red);
                }
            }
        }
    }

    void AuthScreen::printScreen()
    {
        utils::printFromFile(paths::auth);
        io::print("Your ID from save.json: " + std::to_string(controller_.getAppConfig().user.id));
    }

    void AuthScreen::handleLogin() const
    {
        const uint64_t id = io::scanUint64("Enter ID: ");
        const std::string password = io::scanString("Enter password: ");

        if (controller_.loginUser(id, password))
        {
            io::print("Login successful!",io::Color::Green);
            if (const std::optional<UserInfo> user_opt = controller_.getNicknameById(id))
            {
                controller_.getAppConfig().user = *user_opt;
                controller_.getAppConfig().user.password = password;
                if (controller_.saveAppConfig())
                {
                    MainScreen main_screen(controller_);
                    main_screen.run();
                } else
                {
                    io::print("[Error]: Failed to save user in config.",io::Color::Red);
                    io::waitForEnter();
                }
            }
        }
        else
        {
            io::print("[Error]: Server is offline or unreachable. Please check settings.",io::Color::Red);
            io::waitForEnter();
        }
    }

    void AuthScreen::handleRegister() const
    {
        const std::string password = io::scanString("Enter password: ");;

        UserInfo user;
        user.id = controller_.getAppConfig().user.id;
        user.nickname = controller_.getAppConfig().user.nickname;
        user.password = password;

        if (controller_.registerUser(user))
        {
            io::print("Registration successful! You can now login.",io::Color::Green);
            controller_.getAppConfig().user.password = password;
            io::waitForEnter();
        }
        else
        {
            io::print("Registration failed.", io::Color::Red);
            io::waitForEnter();
        }
        if (!controller_.saveAppConfig())
        {
            io::print("[Error]: Failed to save user in config.", io::Color::Red);
        }
    }
}
