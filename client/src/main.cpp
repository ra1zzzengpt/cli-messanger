#include <app/app_controller.hpp>
#include <utils/files/config_storage/storage_controller.hpp>
#include <curl/curl.h>
#include <sodium.h>
#include <random>
#include <utils/logger/logs.hpp>
#include <iostream>
#include "screens/screen_fabric_runner.hpp"

namespace
{
    // YOU CAN CHANGE IT ON YOUR CONFIG FILEPATH
    constexpr auto kConfigPath = "../client/assets/save/save.bin";
}

int main()
{
    if (!stx::log::init())
    {
        std::cout << "[Fatal error]: Failed to initialize logs" << std::endl;
        return 1;
    }
    stx::log::info("application started");
    if (sodium_init() < 0)
    {
        stx::log::error("sodium init failed");
        std::cout << "[Fatal error]: sodium init failed" << std::endl;
        return 1;
    }
    if (const CURLcode init_code = curl_global_init(CURL_GLOBAL_ALL); init_code != CURLE_OK)
    {
        stx::log::error("curl global init failed: " + std::to_string(init_code));
        std::cout << "[Fatal error]: curl init failed: " + std::to_string(init_code) << std::endl;
        return 1;
    }
    app::AppController controller(
        std::make_unique<net::NetworkController>(),
        std::make_unique<stx::StorageController>(kConfigPath),
        std::make_unique<stx::TimeController>()
    );

    if (const std::expected<void, stx::err::Error> result = controller.loadAppConfig(); !result.has_value())
    {
        const stx::err::Error& err = result.error();
        if (!std::holds_alternative<stx::err::ConfigError>(err.type) ||
            std::get<stx::err::ConfigError>(err.type) != stx::err::ConfigError::IncorrectConfiguration)
        {
            stx::log::error("failed to load config: " + err.message);
            return 1;
        }
    }

    if (const auto& user = controller.getAppConfig().user; user.id != 0 && !user.password.empty())
    {
        std::cout << "Checking server status..." << std::endl;

        if (controller.ping().has_value())
        {
            std::cout << "Attempting auto-login..." << std::endl;
            // silent fail — fall through to auth screen if credentials are stale
            if (controller.loginUser(user.id, user.password).has_value())
            {
                stx::log::info("auto-login successful for user id=" + std::to_string(user.id));
                screen::FabricBuilder builder = screen::FabricBuilder(controller,screen::to_int(screen::kScreen::kHello));
                builder.run();
                stx::log::shutdown();
                return 0;
            }
            stx::log::warn("auto-login failed for user id=" + std::to_string(user.id) + ", credentials may be stale");
        }
        else
        {
            stx::log::warn("server is offline or unreachable on startup");
            std::cout << "[Warning]: Server is offline or unreachable. Please check settings." << std::endl;
        }
    }

    screen::FabricBuilder builder = screen::FabricBuilder(controller,screen::to_int(screen::kScreen::kEntry));
    builder.run();

    stx::log::shutdown();
    return 0;
}
