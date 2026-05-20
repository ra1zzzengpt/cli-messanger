#include "app/app_controller.h"
#include "screens/main_screen.h"
#include "screens/auth_screen.h"
#include "utils/files/config_storage/config_storage.h"
#include "api/message_api/httpsapi/https_message_api.h"
#include <curl/curl.h>
#include <sodium.h>

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

    io::check(controller.loadAppConfig(), "[Error]: Failed to load config");

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
