#pragma once
#include "models/app_config.h"

namespace utils
{
    class ConfigStorage final
    {
    public:
        explicit ConfigStorage(std::string filepath);
        ~ConfigStorage() = default;

        [[nodiscard]] std::optional<AppConfig> load() const;

        [[nodiscard]] bool save(const AppConfig& config) const;
    private:
        std::string filepath_;
    };
}