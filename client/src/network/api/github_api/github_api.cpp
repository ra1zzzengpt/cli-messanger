#include <network/api/github_api/github_api.hpp>
#include <network/request_controller/http_response.hpp>
#include <network/request_controller/request_controller.hpp>
namespace
{
    const std::string kCliMessangerRepoUrl = "https://api.github.com/repos/ra1zzzengpt/cli-messanger/releases/latest";
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
        std::expected<HttpResponse,stx::err::Error> resp = curl::RequestController::request(
            curl::RequestMethod::GET,
            url,
            "",
            {
                "Accept: application/vnd.github+json",
                "X-GitHub-Api-Version: 2022-11-28",
                "User-Agent: cli-messanger"
            }
        );
        if (!resp.has_value())
        {
            return std::unexpected(resp.error());
        }
        if (!resp->is_ok())
        {
            return curl::RequestController::httpErr(*resp);
        }
        const auto tag = resp->data.find("tag_name");
        if (tag == resp->data.end() || !tag->is_string())
        {
            return std::unexpected(stx::err::Error{
                stx::err::JsonError::ParsingFailed,
                "GitHub API response does not contain a string tag_name"
            });
        }
        return tag->get<std::string>();
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
