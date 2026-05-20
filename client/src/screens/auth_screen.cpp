#include "auth_screen.h"
#include "utils/console/console.h"
#include "main_screen.h"

#include "server_screen.h"
#include "utils/files/files.h"
#include "utils/files/paths.h"

namespace screen
{
    AuthScreen::AuthScreen(app::AppController& controller) : controller_(controller) {}

    void AuthScreen::run()
    { // todo: load fill missing
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
        io::check(stx::printFromFile(paths::auth), "[Error]: Failed to load auth screen");
        io::print("Your ID from save.json: " + std::to_string(controller_.getAppConfig().user.id));
    }

    void AuthScreen::handleLogin() const
    {
        const uint64_t id = io::scanUint64("Enter ID: ");
        const std::string password = io::scanString("Enter password: ");

        if (!io::check(controller_.loginUser(id, password), "[Error]: Login failed"))
        {
            io::waitForEnter();
            return;
        }
        io::print("Login successful!", io::Color::Green);

        const auto user_opt = controller_.getNicknameById(id);
        if (!io::check(user_opt, "[Error]: Failed to fetch user info"))
        {
            io::waitForEnter();
            return;
        }

        // setLogin saves config internally, no need for a separate saveAppConfig()
        if (!io::check(controller_.setLogin(user_opt.value(), password), "[Error]: Failed to save login"))
        {
            io::waitForEnter();
            return;
        }

        MainScreen main_screen(controller_);
        main_screen.run();
    }

    void AuthScreen::handleRegister() const
    {
        const std::string password = io::scanString("Enter password: ");

        UserInfo user;
        user.id       = controller_.getAppConfig().user.id;
        user.nickname = controller_.getAppConfig().user.nickname;
        user.password = password;

        if (!io::check(controller_.registerUser(user), "[Error]: Registration failed"))
        {
            io::waitForEnter();
            return;
        }
        io::print("Registration successful! You can now login.", io::Color::Green);
        io::check(controller_.updateConfigPassword(password), "[Error]: Failed to save password to config");
        io::waitForEnter();
    }
}
