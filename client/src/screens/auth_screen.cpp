#include <screens/auth_screen.hpp>
#include <utils/console/console.hpp>
#include <screens/main_screen.hpp>

#include <screens/server_screen.hpp>
#include <utils/files/files.hpp>
#include <utils/files/paths.hpp>
#include <utils/logger/logs.hpp>

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
        io::clearConsole();
        io::check(stx::printFromFile(paths::auth), "[Error]: Failed to load auth screen");
        io::print("Your ID from save file: " + std::to_string(controller_.getAppConfig().user.id));
    }

    void AuthScreen::handleLogin() const
    {
        const uint64_t id = io::scanUint64("Enter ID: ");
        const std::string password = io::scanString("Enter password: ");

        stx::log::info("login attempt for user id=" + std::to_string(id));
        if (!io::check(controller_.loginUser(id, password), "[Error]: Login failed"))
        {
            io::waitForEnter();
            return;
        }
        stx::log::info("login successful for user id=" + std::to_string(id));
        io::print("Login successful!", io::Color::Green);

        const auto user_opt = controller_.getNicknameById(id);
        if (!io::check(user_opt, "[Error]: Failed to fetch user info"))
        {
            io::waitForEnter();
            return;
        }

        if (!io::check(controller_.setLogin(user_opt.value(), password), "[Error]: Failed to save login"))
        {
            io::waitForEnter();
            io::clearConsole();
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

        stx::log::info("registration attempt for user id=" + std::to_string(user.id));
        if (!io::check(controller_.registerUser(user), "[Error]: Registration failed"))
        {
            io::waitForEnter();
            return;
        }
        stx::log::info("registration successful for user id=" + std::to_string(user.id));
        io::print("Registration successful! You can now login.", io::Color::Green);
        io::check(controller_.updateConfigPassword(password), "[Error]: Failed to save password to config");
        io::waitForEnter();
        io::clearConsole();
    }
}
