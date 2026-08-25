#ifndef APP_CONTROLLER_HPP
#define APP_CONTROLLER_HPP

#include <expected>
#include <network/network_controller/i_network_controller.hpp>
#include <models/app_config.hpp>
#include <utils/files/config_storage/i_storage_controller.hpp>
#include <utils/time/i_time_controller.hpp>
#include <models/message.hpp>

namespace app
{

    class AppController final
    {
    public:
        // - OBJECT -
        AppController(std::unique_ptr<net::INetworkController> net,
            std::unique_ptr<stx::IStorageController> storage, std::unique_ptr<stx::ITimeController> time);
        ~AppController() = default;
        AppController(AppController&) = delete;
        AppController(AppController&&) = delete;
        AppController& operator=(const AppController&) = delete;
        AppController& operator=(AppController&&) = delete;

        // - UPDATERS -
        [[nodiscard]] std::expected<void,stx::err::Error> updatePassword(const std::string& new_password) const;
        [[nodiscard]] std::expected<void,stx::err::Error> updateNickname(const std::string& new_nickname) const;
        // [[nodiscard]] std::expected<void,stx::err::Error> updateID(const uint64_t& new_id) const;
        std::expected<void,stx::err::Error> updateUrl(const std::string& new_url);

        std::expected<void, stx::err::Error> relogging(const std::string& new_url);

        // - ONLY STORAGE FUNCTIONAL -
        [[nodiscard]] const AppConfig& getAppConfig() const noexcept;
        [[nodiscard]] const std::vector<ChatInfo>& getChats() const;
        std::expected<void,stx::err::Error> loadAppConfig();
        std::expected<void,stx::err::Error> saveAppConfig();
        std::expected<void,stx::err::Error> addChat(const ChatInfo& new_chat);

        // - ONLY NETWORK FUNCTIONAL -
        [[nodiscard]] std::expected<std::string,stx::err::Error> ping() const;
        [[nodiscard]] std::expected<std::string,stx::err::Error> ping(const std::string& another_url) const;
        [[nodiscard]] std::expected<std::vector<Message>,stx::err::Error> getMessages(const UserInfo& other_user) const;
        [[nodiscard]] std::expected<void,stx::err::Error> sendMessage(const UserInfo& other_user, const std::string& text) const;
        [[nodiscard]] std::expected<std::string,stx::err::Error> getNicknameById(std::uint64_t id) const;
        [[nodiscard]] std::expected<void,stx::err::Error> registerUser(const UserInfo& user) const;
        [[nodiscard]] std::expected<void,stx::err::Error> loginUser(std::uint64_t id, const std::string& password) const;

        // - NETWORK FROM ANOTHER API -
        [[nodiscard]] bool versionControl() const;
        [[nodiscard]] const std::string& currentVersion() const;
        [[nodiscard]] const std::string& lastestVersion() const;

        // - TIME -
        [[nodiscard]] bool canMakeRequest() const;
        bool tryAcquireRequest();
    private:
        std::unique_ptr<net::INetworkController> networkController_;
        std::unique_ptr<stx::IStorageController> storageController_;
        std::unique_ptr<stx::ITimeController> timeController_;
    };
}

#endif
