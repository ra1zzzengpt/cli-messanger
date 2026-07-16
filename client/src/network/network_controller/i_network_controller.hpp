#pragma once

#include <network/api/messanger_api/https_api.hpp>
#include <network/api/github_api/github_api.hpp>
#include <utils/error/app_error.hpp>
#include <models/user_info.hpp>
#include <models/message.hpp>
#include <vector>
#include <string>
#include <expected>

namespace net
{
    class INetworkController
    {
    public:
        virtual ~INetworkController() = default;

        // - MSNGR -
        [[nodiscard]] virtual std::expected<std::string,stx::err::Error> ping() const = 0;
        [[nodiscard]] virtual std::expected<std::string,stx::err::Error> ping(const std::string& another_url) const = 0;

        [[nodiscard]] virtual std::expected<void,stx::err::Error> registerUser(
            std::uint64_t id,
            const std::string& nick,
            const std::string& password
        ) const = 0;

        [[nodiscard]] virtual std::expected<void,stx::err::Error> loginUser(
            std::uint64_t id,
            const std::string& password
        ) const = 0;

        [[nodiscard]] virtual std::expected<std::string,stx::err::Error> getUsernameById(
            std::uint64_t id
        ) const = 0;

        [[nodiscard]] virtual std::expected<void,stx::err::Error> updatePassword(
            std::uint64_t id,
            const std::string& currentPassword,
            const std::string& newPassword
        ) const = 0;

        [[nodiscard]] virtual std::expected<void,stx::err::Error> updateNickname(
            std::uint64_t id,
            const std::string& password,
            const std::string& newNick
        ) const = 0;

        [[nodiscard]] virtual std::expected<void,stx::err::Error> sendMessage(
            std::uint64_t fromId,
            std::uint64_t toId,
            const std::string& password,
            const std::string& text
        ) const = 0;


        [[nodiscard]] virtual std::expected<std::vector<Message>,stx::err::Error> dumpMessages(
            std::uint64_t myId,
            std::uint64_t peerId,
            const std::string& password
        ) const = 0;

        virtual void setUrlMessageApi(const std::string& url) = 0;

        // -GHB-
        [[nodiscard]] virtual bool verifyVersion() const = 0;

        [[nodiscard]] virtual const std::string& currentVersion() const = 0;
        [[nodiscard]] virtual  const std::string& lastestVersion() const = 0;
    };
}