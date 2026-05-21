#include "app/app_controller.h"
#include "screens/main_screen.h"
#include "screens/auth_screen.h"
#include "utils/files/config_storage/config_storage.h"
#include "api/message_api/httpsapi/https_message_api.h"
#include <curl/curl.h>
#include <sodium.h>
#include <random>

#include "utils/console/console.h"

namespace
{
    // YOU CAN CHANGE IT ON YOUR CONFIG FILEPATH
    constexpr auto kConfigPath = "../client/assets/save/save.json";
}

int main()
{
    if (sodium_init() < 0)
    {
        io::print("[Fatal error]: sodium init failed", io::Color::Red);
        return 1;
    }
    if (const CURLcode init_code = curl_global_init(CURL_GLOBAL_ALL); init_code != CURLE_OK)
    {
        io::print("[Fatal error]: curl init failed: " + std::to_string(init_code));
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
            io::print("[Error]: Failed to load config: " + err.message, io::Color::Red);
            return 1;
        }
        const std::string url = io::scanString("Server URL: ");
        const std::string nickname = io::scanString("Your nickname: ");
        std::mt19937_64 gen(std::random_device{}());
        const uint64_t id = std::uniform_int_distribution<uint64_t>{}(gen);
        io::check(controller.updateUrl(url),"[Error]: Failed to save server URL");
        io::check(controller.setupInitialUser(id, nickname),"[Error]: Failed to save user info");
    }

    if (const auto& user = controller.getAppConfig().user; user.id != 0 && !user.password.empty())
    {
        io::print("Checking server status...");

        if (controller.ping().has_value())
        {
            io::print("Attempting auto-login...");
            // silent fail — fall through to auth screen if credentials are stale
            if (controller.loginUser(user.id, user.password).has_value())
            {
                screen::MainScreen mainMenu(controller);
                mainMenu.run();
                return 0;
            }
        }
        else
        {
            io::print("[Warning]: Server is offline or unreachable. Please check settings.", io::Color::Yellow);
            io::waitForEnter();
        }
    }

    screen::AuthScreen authScreen(controller);
    authScreen.run();

    return 0;
}
