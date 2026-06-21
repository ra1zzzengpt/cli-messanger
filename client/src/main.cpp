#include "app/app_controller.hpp"
#include "screens/main_screen.hpp"
#include "screens/auth_screen.hpp"
#include "utils/files/config_storage/config_storage.hpp"
#include "api/message_api/httpsapi/https_message_api.hpp"
#include <curl/curl.h>
#include <sodium.h>
#include <random>

#include "utils/console/console.hpp"
#include "utils/logger/logs.hpp"

namespace
{
    // YOU CAN CHANGE IT ON YOUR CONFIG FILEPATH
    constexpr auto kConfigPath = "../client/assets/save/save.bin";
}

int main()
{
    if (!stx::log::init())
    {
        io::print("[Fatal error]: Failed to initialize logs", io::Color::Red);
        return 1;
    }
    stx::log::info("application started");
    if (sodium_init() < 0)
    {
        stx::log::error("sodium init failed");
        io::print("[Fatal error]: sodium init failed", io::Color::Red);
        return 1;
    }
    if (const CURLcode init_code = curl_global_init(CURL_GLOBAL_ALL); init_code != CURLE_OK)
    {
        stx::log::error("curl global init failed: " + std::to_string(init_code));
        io::print("[Fatal error]: curl init failed: " + std::to_string(init_code),io::Color::Red);
        return 1;
    }
    app::AppController controller(
        std::make_unique<api::HttpMessageApi>(),
        std::make_unique<stx::ConfigStorage>(kConfigPath)
    );

    if (const auto result = controller.loadAppConfig(); !result.has_value())
    {
        const auto& err = result.error();
        if (!std::holds_alternative<stx::err::ConfigError>(err.type) ||
            std::get<stx::err::ConfigError>(err.type) != stx::err::ConfigError::IncorrectConfiguration)
        {
            stx::log::error("failed to load config: " + err.message);
            io::print("[Error]: Failed to load config: " + err.message, io::Color::Red);
            return 1;
        }
        const std::string url = io::scanString("Server URL: ");
        const std::string nickname = io::scanString("Your nickname: ");
        // WARNING: THIS WILL BE MOVED ON SERVER IN USER REGISTER TODO: 1.0?
        std::mt19937_64 gen(std::random_device{}());
        const uint64_t id = std::uniform_int_distribution<uint64_t>{}(gen);
        // ----------------------------------------------------
        io::check(controller.updateUrl(url),"[Error]: Failed to save server URL");
        io::check(controller.setupInitialUser(id, nickname),"[Error]: Failed to save user info");
    }

    if (const auto& user = controller.getAppConfig().user; user.id != 0 && !user.password.empty())
    {
        io::print("Checking server status...",io::Color::Yellow);

        if (controller.ping().has_value())
        {
            io::print("Attempting auto-login...",io::Color::Yellow);
            // silent fail — fall through to auth screen if credentials are stale
            if (controller.loginUser(user.id, user.password).has_value())
            {
                stx::log::info("auto-login successful for user id=" + std::to_string(user.id));
                screen::MainScreen mainMenu(controller);
                mainMenu.run();
                return 0;
            }
            stx::log::warn("auto-login failed for user id=" + std::to_string(user.id) + ", credentials may be stale");
        }
        else
        {
            stx::log::warn("server is offline or unreachable on startup");
            io::print("[Warning]: Server is offline or unreachable. Please check settings.", io::Color::Yellow);
            io::waitForEnter();
        }
    }

    screen::AuthScreen authScreen(controller);
    authScreen.run();

    stx::log::shutdown();
    return 0;
}
