#include "config_storage.h"

#include "utils/console/console.h"

#include <fstream>
#include <filesystem>
#include <random>
#include <utility>

namespace utils {
    namespace {
        uint64_t randomUint64()
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution dist(
                std::numeric_limits<uint64_t>::min(),
                std::numeric_limits<uint64_t>::max()
                );
            return dist(gen);
        };

        bool hasDefaultValues(const AppConfig& config)
        {
            return config.server.host.empty()
                || config.server.port == 0
                || config.user.id == 0
                || config.user.nickname.empty();
        }

        void fillMissingConfigValues(AppConfig& config)
        {
            if (config.server.host.empty())
            {
                config.server.host = io::scanString("Server host: ");
            }

            if (config.server.port == 0)
            {
                config.server.port = static_cast<int>(io::scanUint32("Server port: "));
            }

            if (config.user.id == 0)
            {
                config.user.id = randomUint64();
            }

            if (config.user.nickname.empty())
            {
                config.user.nickname = io::scanString("User nick: ");
            }
        }
    }

    ConfigStorage::ConfigStorage(std::string filepath) : filepath_(std::move(filepath))
    { }

    bool ConfigStorage::save(const AppConfig& config) const
    {

        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            io::print("[Error]: Cannot create config directory", io::Color::Red);
            return false;
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            io::print("[Error]: Cannot save config file", io::Color::Red);
            return false;
        }

        file << nlohmann::json(config).dump(4);
        return true;
    }

    std::optional<AppConfig> ConfigStorage::load() const
    {
        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            io::print("[FATAL ERROR]: Cannot create config directory", io::Color::Red);
            return std::nullopt;
        }

        std::ifstream file(path);
        AppConfig config;

        if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
        {
            try
            {
                nlohmann::json json = nlohmann::json::parse(file);
                config = json.get<AppConfig>();
            }
            catch (const nlohmann::json::exception& ex)
            {
                io::print("[Error]: Cannot parse config file: " + std::string(ex.what()), io::Color::Red);
                config = AppConfig{};
            }
        }

        if (hasDefaultValues(config))
        {
            io::print("Config is incomplete. Please enter missing values:", io::Color::Yellow);
            fillMissingConfigValues(config);
            if (!save(config))
            {
                io::print("[Error]: Failed to save updated config", io::Color::Red);
            }
        }

        return config;
    }
}
