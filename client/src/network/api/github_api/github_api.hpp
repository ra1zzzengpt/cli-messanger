#pragma once
namespace api
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


    };
}