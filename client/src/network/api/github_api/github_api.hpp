#ifndef GIT_HUB_API_HPP
#define GIT_HUB_API_HPP

#include <string>
#include <expected>
#include <utils/error/app_error.hpp>
namespace net::api
{
    class GitHubApi final
    {
    public:
        GitHubApi();
        ~GitHubApi() = default;
        GitHubApi(const GitHubApi&) = delete;
        GitHubApi& operator=(const GitHubApi&) = delete;
        GitHubApi(GitHubApi&&) = delete;
        GitHubApi& operator=(GitHubApi&&) = delete;

        [[nodiscard]] bool lastestVersionControl() const;

        [[nodiscard]] const std::string& currentVersion() const;
        [[nodiscard]] const std::string& lastestVersion() const;
    private:
        std::string currentVersion_, lastestVersion_;
        static std::expected<std::string,stx::err::Error> lastestReleaseTag(const std::string& url);
    };
}

#endif