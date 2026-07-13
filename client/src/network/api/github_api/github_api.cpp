#include <network/api/github_api/github_api.hpp>
#include <network/request_controller/http_response.hpp>
#include <network/request_controller/request_controller.hpp>
namespace
{
    const std::string kCliMessangerRepoUrl = "https://api.github.com/repos/ra1zzzengpt/cli-messanger/releases/lates";
    const std::string kCurrentVersion = "v1.0";
}
namespace net::api
{
    GitHubApi::GitHubApi()
    {
        currentVersion_ = kCurrentVersion;
        if (const std::expected<std::string, stx::err::Error> temp = lastestReleaseTag(kCliMessangerRepoUrl); temp.has_value())
        {
            lastestVersion_ = temp.value();
        } else
        {
            lastestVersion_ = temp.error().message;
        }
    }

    std::expected<std::string,stx::err::Error> GitHubApi::lastestReleaseTag(const std::string& url)
    {
        std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(curl::RequestMethod::GET,url,"",{"Accept: application/vnd.github+json", "X-GitHub-Api-Version: 2026-03-10"});
        if (!resp.has_value())
        {
            return std::unexpected(resp.error());
        }
        if (!resp->is_ok() || resp->data.value("ok",false))
        {
            return curl::RequestController::httpErr(*resp);
        }
        return resp->data["tag_name"].get<std::string>();
    }

    bool GitHubApi::lastestVersionControl() const
    {
        if (lastestVersion_ == currentVersion_)
        {
            return true;
        }
        return false;
    }

    const std::string& GitHubApi::currentVersion() const
    {
        return currentVersion_;
    }

    const std::string &GitHubApi::lastestVersion() const
    {
        return lastestVersion_;
    }
}
