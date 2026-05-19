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
            return config.server.url.empty()
                || config.user.id == 0
                || config.user.nickname.empty();
        }

        void fillMissingConfigValues(AppConfig& config)
        {
            if (config.server.url.empty())
            {
                config.server.url = io::scanString("Server url: ");
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

    bool ConfigStorage::save()
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

        file << nlohmann::json(cryptoSodium_.encode(nlohmann::json(config_).dump())).dump(4);
        return true;
    }

    void ConfigStorage::load()
    {
        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            io::print("[FATAL ERROR]: Cannot create config directory", io::Color::Red);
            std::terminate();
        }

        std::ifstream file(path);
        AppConfig config;

        if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
        {
            try
            {
                nlohmann::json crypted_file = nlohmann::json::parse(file);
                std::optional<std::string> decoded = cryptoSodium_.decode(crypted_file);
                if (!decoded.has_value())
                {
                    throw std::runtime_error("Decode failed");
                }
                config = nlohmann::json::parse(*decoded).get<AppConfig>();
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
        }

        config_ = config;
        if (!save())
        {
            io::print("[Error]: Failed to save updated config", io::Color::Red);
        }
    }

    const AppConfig& ConfigStorage::getConfig() const noexcept
    {
        return config_;
    }

    void ConfigStorage::setByLogin(const UserInfo& user, const std::string& password)
    {
        config_.user = user;
        config_.user.password = password;
        // todo: save check to expected
    }

    void ConfigStorage::updatePassword(const std::string &new_password)
    {
        config_.user.password = new_password;
        // todo: save check to expected
    }

    void ConfigStorage::updateNickname(const std::string& new_nickname)
    {
        config_.user.nickname = new_nickname;
        // todo: save check to expected
    }

    void ConfigStorage::addChat(const ChatInfo &new_chat)
    {
        config_.chats.push_back(new_chat);
        // todo: save check to expected
    }

    void ConfigStorage::updateUrl(const std::string &new_url)
    {
        config_.server.url = new_url;
        // todo: save check to expected
    }
}
