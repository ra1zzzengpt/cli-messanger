#include "config_storage.h"

#include <expected>

#include <fstream>
#include <filesystem>
#include <utility>

namespace stx {
    namespace {
        bool hasDefaultValues(const AppConfig& config)
        {
            return config.server.url.empty()
                || config.user.id == 0
                || config.user.nickname.empty();
        }
    }

    ConfigStorage::ConfigStorage(std::string filepath) : filepath_(std::move(filepath))
    { }

    std::expected<void,err::AppError> ConfigStorage::save()
    {

        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            return std::unexpected(err::AppError{err::ConfigError::CreateDirectoryFailed,"can't create directory: " + std::string(path) + " (" + error.message() + ")"});
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            return std::unexpected(err::AppError{err::ConfigError::OpenFileFailed});
        }
        std::expected<CryptoInfo,err::AppError> encrypted_result = cryptoSodium_.encode(nlohmann::json(config_).dump());
        if (!encrypted_result.has_value())
        {
            return std::unexpected(encrypted_result.error());
        }
        file << nlohmann::json(encrypted_result.value()).dump(4);
        return {};
    }

    std::expected<void,err::AppError> ConfigStorage::load()
    {
        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            return std::unexpected(err::AppError{err::ConfigError::CreateDirectoryFailed,
                "can't create directory: " + std::string(path) + " (" + error.message() + ")"});
        }

        std::ifstream file(path);
        AppConfig config;

        if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
        {
            try
            {
                nlohmann::json crypted_file = nlohmann::json::parse(file);
                std::expected<std::string,err::AppError> decoded = cryptoSodium_.decode(crypted_file);
                if (!decoded.has_value())
                {
                    return std::unexpected(decoded.error());
                }
                config = nlohmann::json::parse(*decoded).get<AppConfig>();
            }
            catch (const nlohmann::json::exception&)
            {
                return std::unexpected(err::AppError{err::JsonError::ParsingFailed,"can't parse config"});
            }
        }

        if (hasDefaultValues(config))
        {
            return std::unexpected(err::AppError{err::ConfigError::IncorrectConfiguration,"not complete"});
        }

        config_ = config;
        return save();
    }

    const AppConfig& ConfigStorage::getConfig() const noexcept
    {
        return config_;
    }

    std::expected<void,err::AppError> ConfigStorage::setByLogin(const UserInfo& user, const std::string& password)
    {
        config_.user = user;
        config_.user.password = password;
        return save();
    }

    std::expected<void,err::AppError> ConfigStorage::updatePassword(const std::string &new_password)
    {
        config_.user.password = new_password;
        return save();
    }

    std::expected<void,err::AppError> ConfigStorage::updateNickname(const std::string& new_nickname)
    {
        config_.user.nickname = new_nickname;
        return save();
    }

    std::expected<void,err::AppError> ConfigStorage::addChat(const ChatInfo &new_chat)
    {
        config_.chats.push_back(new_chat);
        return save();
    }

    std::expected<void,err::AppError> ConfigStorage::updateUrl(const std::string &new_url)
    {
        config_.server.url = new_url;
        return save();
    }

    std::expected<void,err::AppError> ConfigStorage::setInitialUser(uint64_t id, const std::string& nickname)
    {
        config_.user.id = id;
        config_.user.nickname = nickname;
        return save();
    }
}
