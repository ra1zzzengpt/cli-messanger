#include "config_storage.h"

#include <expected>

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

    std::expected<void,errors::AppError> ConfigStorage::save()
    {

        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            return std::unexpected(errors::AppError{errors::ConfigError::CreateDirectoryFailed,"can't create directory: " + std::string(path) + " (" + error.message() + ")"});
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            return std::unexpected(errors::AppError{errors::ConfigError::OpenFileFailed});
        }
        std::expected<CryptoInfo,errors::AppError> encrypted_result = cryptoSodium_.encode(nlohmann::json(config_).dump());
        if (!encrypted_result.has_value())
        {
            return std::unexpected(encrypted_result.error());
        }
        file << nlohmann::json(nlohmann::json(encrypted_result.value()).dump()).dump(4);
        return {};
    }

    std::expected<void,errors::AppError> ConfigStorage::load()
    {
        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            return std::unexpected(errors::AppError{errors::ConfigError::CreateDirectoryFailed,
                "can't create directory: " + std::string(path) + " (" + error.message() + ")"});
        }

        std::ifstream file(path);
        AppConfig config;

        if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
        {
            try
            {
                nlohmann::json crypted_file = nlohmann::json::parse(file);
                std::expected<std::string,errors::AppError> decoded = cryptoSodium_.decode(crypted_file);
                if (!decoded.has_value())
                {
                    return std::unexpected(decoded.error());
                }
                config = nlohmann::json::parse(*decoded).get<AppConfig>();
            }
            catch (const nlohmann::json::exception& ex)
            {
                config = AppConfig{};
            }
        }

        // TODO: WHAT'S WITH IT
        // SRP PROBLEM
        if (hasDefaultValues(config))
        {
            io::print("Config is incomplete. Please enter missing values:", io::Color::Yellow);
            fillMissingConfigValues(config);
        }

        config_ = config;
        if (std::expected<void,errors::AppError> save_result = save(); !save_result.has_value())
        {
            return std::unexpected(save_result.error());
        }
        return {};
    }

    const AppConfig& ConfigStorage::getConfig() const noexcept
    {
        return config_;
    }

    std::expected<void,errors::AppError> ConfigStorage::setByLogin(const UserInfo& user, const std::string& password)
    {
        config_.user = user;
        config_.user.password = password;
        if (std::expected<void,errors::AppError> save_result = save(); !save_result.has_value())
        {
            return std::unexpected(save_result.error());
        }
        return {};
    }

    std::expected<void,errors::AppError> ConfigStorage::updatePassword(const std::string &new_password)
    {
        config_.user.password = new_password;
        if (std::expected<void,errors::AppError> save_result = save(); !save_result.has_value())
        {
            return std::unexpected(save_result.error());
        }
        return {};
    }

    std::expected<void,errors::AppError> ConfigStorage::updateNickname(const std::string& new_nickname)
    {
        config_.user.nickname = new_nickname;
        if (std::expected<void,errors::AppError> save_result = save(); !save_result.has_value())
        {
            return std::unexpected(save_result.error());
        }
        return {};
    }

    std::expected<void,errors::AppError> ConfigStorage::addChat(const ChatInfo &new_chat)
    {
        config_.chats.push_back(new_chat);
        if (std::expected<void,errors::AppError> save_result = save(); !save_result.has_value())
        {
            return std::unexpected(save_result.error());
        }
        return {};
    }

    std::expected<void,errors::AppError> ConfigStorage::updateUrl(const std::string &new_url)
    {
        config_.server.url = new_url;
        if (std::expected<void,errors::AppError> save_result = save(); !save_result.has_value())
        {
            return std::unexpected(save_result.error());
        }
        return {};
    }
}
