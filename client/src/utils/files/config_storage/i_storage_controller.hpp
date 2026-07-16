#pragma once
#include <expected>

#include <models/app_config.hpp>
#include <utils/error/app_error.hpp>

namespace stx
{
    class IStorageController
    {
    public:
        virtual ~IStorageController() = default;

        [[nodiscard]] virtual const AppConfig& getConfig() const noexcept = 0;

        virtual std::expected<void,err::Error> load() = 0;

        virtual std::expected<void,err::Error> save() = 0;

        virtual void clear() = 0;

        // - USER -
        virtual std::expected<void,err::Error> updateUser(const UserInfo& user) = 0;
        virtual std::expected<void,err::Error> updatePassword(const std::string& new_password) = 0;
        virtual std::expected<void,err::Error> updateNickname(const std::string& new_nickname) = 0;
        virtual std::expected<void,err::Error> updateID(const uint64_t& new_id) = 0;

        // - SERVER -
        virtual std::expected<void,err::Error> updateUrl(const std::string& new_url) = 0;

        // - CHATS -
        virtual std::expected<void,err::Error> addChat(const ChatInfo& new_chat) = 0;
    };
}
