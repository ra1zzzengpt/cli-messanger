#include <utils/files/config_storage/config_storage.hpp>

#include <expected>

#include <fstream>
#include <filesystem>
#include <utility>

#include <utils/logger/logs.hpp>

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

    std::expected<void,err::Error> ConfigStorage::save()
    {

        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            log::error("config save: can't create directory " + std::string(path) + " (" + error.message() + ")");
            return std::unexpected(err::Error{err::ConfigError::CreateDirectoryFailed,"can't create directory: " + std::string(path) + " (" + error.message() + ")"});
        }

        std::ofstream file(path,std::ios::binary);
        if (!file.is_open())
        {
            log::error("config save: can't open file " + std::string(path));
            return std::unexpected(err::Error{err::ConfigError::OpenFileFailed});
        }
        std::expected<CryptoInfo,err::Error> encrypted_result = cryptoSodium_.encode(nlohmann::json(config_).dump());
        if (!encrypted_result.has_value())
        {
            log::error("config save: encryption failed (" + encrypted_result.error().message + ")");
            return std::unexpected(encrypted_result.error());
        }
        std::vector<unsigned char> export_ready = to_export(encrypted_result.value());
        file.write(reinterpret_cast<const std::ostream::char_type *>(export_ready.data()), static_cast<long>(export_ready.size()));
        log::info("config saved to " + std::string(path));
        return {};
    }

    std::expected<void,err::Error> ConfigStorage::load()
    {
        const std::filesystem::path path{filepath_};
        std::error_code error;
        std::filesystem::create_directories(path.parent_path(), error);

        if (error)
        {
            log::error("config load: can't create directory " + std::string(path) + " (" + error.message() + ")");
            return std::unexpected(err::Error{err::ConfigError::CreateDirectoryFailed,
                "can't create directory: " + std::string(path) + " (" + error.message() + ")"});
        }

        std::ifstream file(path,std::ios::binary);
        AppConfig config;

        if (file.is_open() && file.peek() != std::ifstream::traits_type::eof())
        {
            try
            {
                file.seekg(0, std::ios::end);
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                std::vector<unsigned char> crypted_file(size);
                file.read(reinterpret_cast<std::istream::char_type *>(crypted_file.data()),static_cast<long>(crypted_file.size()));
                std::expected<std::string,err::Error> decoded = cryptoSodium_.decode(import(crypted_file));
                if (!decoded.has_value())
                {
                    log::error("config load: decryption failed (" + decoded.error().message + ")");
                    return std::unexpected(decoded.error());
                }
                config = nlohmann::json::parse(*decoded).get<AppConfig>();
            }
            catch (...)
            {
                log::error("config load: can't parse save file " + std::string(path));
                return std::unexpected(err::Error{err::JsonError::ParsingFailed,"can't parse save file"});
            }
        }

        if (hasDefaultValues(config))
        {
            log::info("config is incomplete, initial setup is required");
            return std::unexpected(err::Error{err::ConfigError::IncorrectConfiguration,"not complete"});
        }

        config_ = config;
        log::info("config loaded successfully (user id=" + std::to_string(config_.user.id) + ")");
        return save();
    }

    const AppConfig& ConfigStorage::getConfig() const noexcept
    {
        return config_;
    }

    std::expected<void,err::Error> ConfigStorage::setByLogin(const UserInfo& user, const std::string& password)
    {
        config_.user = user;
        config_.user.password = password;
        return save();
    }

    std::expected<void,err::Error> ConfigStorage::updatePassword(const std::string &new_password)
    {
        config_.user.password = new_password;
        return save();
    }

    std::expected<void,err::Error> ConfigStorage::updateNickname(const std::string& new_nickname)
    {
        config_.user.nickname = new_nickname;
        return save();
    }

    std::expected<void,err::Error> ConfigStorage::addChat(const ChatInfo &new_chat)
    {
        config_.chats.push_back(new_chat);
        return save();
    }

    std::expected<void,err::Error> ConfigStorage::updateUrl(const std::string &new_url)
    {
        config_.server.url = new_url;
        return save();
    }

    std::expected<void,err::Error> ConfigStorage::setInitialUser(const uint64_t id, const std::string& nickname)
    {
        config_.user.id = id;
        config_.user.nickname = nickname;
        return save();
    }
}
