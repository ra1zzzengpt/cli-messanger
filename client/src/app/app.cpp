#include "app.h"

#include "screens/main_screen.h"
#include "utils/console/console.h"

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace {
    constexpr const char* kConfigPath = "client/assets/save/save.json";

    bool HasDefaultValues(const AppConfig& config) {
        return config.server.host.empty()
            || config.server.port == 0
            || config.user.id == 0
            || config.user.nick.empty();
    }

    std::string ScanString(const std::string_view prompt) {
        std::string value;

        while (value.empty()) {
            io::print(prompt, io::COLOR::WHITE, "");
            std::getline(std::cin, value);

            if (value.empty()) {
                io::print("[Error]: Value cannot be empty", io::COLOR::RED);
            }
        }

        return value;
    }

    void FillMissingConfigValues(AppConfig& config) {
        if (config.server.host.empty()) {
            // todo replace with ScanHost()
            config.server.host = ScanString("Server host: ");
        }

        if (config.server.port == 0) {
            // todo replace with ScanPort()
            config.server.port = static_cast<int>(io::SafeScanUint32("Server port: "));
        }

        if (config.user.id == 0) {
            // todo replace with ScanUserId() or registration request to server
            config.user.id = io::SafeScanUint32("User id: ");
        }

        if (config.user.nick.empty()) {
            // todo replace with ScanNick()
            config.user.nick = ScanString("User nick: ");
        }
    }

    bool SaveConfig(const AppConfig& config) {
        const std::filesystem::path path{kConfigPath};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error) {
            io::print("[Error]: Cannot create config directory", io::COLOR::RED);
            return false;
        }

        std::ofstream file(path);
        if (!file.is_open()) {
            io::print("[Error]: Cannot save config file", io::COLOR::RED);
            return false;
        }

        file << nlohmann::json(config).dump(4);
        return true;
    }
}

void Application::Run() {
    if (!LoadConfig()) {
        io::print("[Error]: Cannot load config", io::COLOR::RED);
        return;
    }

    screen::MainScreen mainScreen(config);
    mainScreen.run();
}

bool Application::LoadConfig() {
    std::ifstream file(kConfigPath);

    if (file.is_open() && file.peek() != std::ifstream::traits_type::eof()) {
        try {
            const nlohmann::json json = nlohmann::json::parse(file);
            config = json.get<AppConfig>();
        }
        catch (const nlohmann::json::exception& exception) {
            io::print(std::string("[Error]: Cannot parse config file: ") + exception.what(), io::COLOR::RED);
            config = AppConfig{};
        }
    }

    if (HasDefaultValues(config)) {
        io::print("Config is incomplete. Please enter missing values:", io::COLOR::YELLOW);
        FillMissingConfigValues(config);
        return SaveConfig(config);
    }

    return true;
}
