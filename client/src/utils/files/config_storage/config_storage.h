#pragma once
#include "models/app_config.h"

namespace utils
{
    class ConfigStorage final
    {
    public:
        explicit ConfigStorage(std::string filepath);
        ~ConfigStorage() = default;

        [[nodiscard]] std::optional<AppConfig> Load() const;

        [[nodiscard]] bool Save(const AppConfig& config) const;
    private:
        std::string filepath_;
    };
}