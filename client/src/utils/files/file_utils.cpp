#include "file_utils.h"

#include "utils/console/console.h"

#include <fstream>
#include <filesystem>
#include <random>

namespace utils {
    namespace {
        constexpr const char* kConfigPath = "client/assets/save/save.json";

        uint64_t RandomUInt64()
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution dist(
                std::numeric_limits<uint64_t>::min(),
                std::numeric_limits<uint64_t>::max()
                );
            return dist(gen);
        };

        bool HasDefaultValues(const AppConfig& config) {
            return config.server.host.empty()
                || config.server.port == 0
                || config.user.id == 0
                || config.user.nick.empty();
        }

        void FillMissingConfigValues(AppConfig& config) {
            if (config.server.host.empty()) {
                config.server.host = io::ScanString("Server host: ");
            }

            if (config.server.port == 0) {
                config.server.port = static_cast<int>(io::ScanUint32("Server port: "));
            }

            config.user.id = RandomUInt64();

            if (config.user.nick.empty()) {
                config.user.nick = io::ScanString("User nick: ");
            }
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

    bool LoadConfig(AppConfig& config) {
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
}