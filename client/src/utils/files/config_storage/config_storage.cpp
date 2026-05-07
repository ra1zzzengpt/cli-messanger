#include "config_storage.h"

#include "utils/console/console.h"

#include <fstream>
#include <filesystem>
#include <random>
#include <utility>

namespace utils {
    namespace {
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

        bool HasDefaultValues(const AppConfig& config)
        {
            return config.server.host.empty()
                || config.server.port == 0
                || config.user.id == 0
                || config.user.nickname.empty();
        }

        void FillMissingConfigValues(AppConfig& config)
        {
            if (config.server.host.empty())
            {
                config.server.host = io::ScanString("Server host: ");
            }

            if (config.server.port == 0)
            {
                config.server.port = static_cast<int>(io::ScanUint32("Server port: "));
            }

            if (config.user.id == 0)
            {
                config.user.id = RandomUInt64();
            }

            if (config.user.nickname.empty())
            {
                config.user.nickname = io::ScanString("User nick: ");
            }
        }
    }

    ConfigStorage::ConfigStorage(std::string filepath) : filepath_(std::move(filepath))
    { }

    bool ConfigStorage::Save(const AppConfig& config) const
    {

        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            io::print("[Error]: Cannot create config directory", io::COLOR::RED);
            return false;
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            io::print("[Error]: Cannot save config file", io::COLOR::RED);
            return false;
        }

        file << nlohmann::json(config).dump(4);
        return true;
    }

    std::optional<AppConfig> ConfigStorage::Load() const
    {
        std::ifstream file(filepath_);
        if (!file.is_open())
            return std::nullopt; // todo fix for fatal error with creating directory

        AppConfig config;

        if (file.peek() != std::ifstream::traits_type::eof())
        {
            try
            {
                nlohmann::json json = nlohmann::json::parse(file);
                config = json.get<AppConfig>();
            }
            catch (const nlohmann::json::exception& ex)
            {
                io::print("[Error]: Cannot parse config file: " + std::string(ex.what()), io::COLOR::RED);
                return std::nullopt; // битый файл — возвращаем ошибку
            }
        }

        if (HasDefaultValues(config))
        {
            io::print("Config is incomplete. Please enter missing values:", io::COLOR::YELLOW);
            FillMissingConfigValues(config);
            if (!Save(config))
            {
                io::print("[Error]: Failed to save updated config", io::COLOR::RED);
            }
        }

        return config;
    }
}
