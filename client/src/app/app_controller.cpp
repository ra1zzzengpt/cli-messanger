#include "app_controller.h"

#include "screens/main_screen.h"
#include "utils/console/console.h"
#include "utils/files/file_utils.h"

#include <nlohmann/json.hpp>
namespace app
{
    AppController::AppController(
        std::unique_ptr<api::IMessageApi> api,
        std::unique_ptr<utils::ConfigStorage> storage
        ) : messageApi_(std::move(api)), configStorage_(std::move(storage))
    { }

    AppConfig& AppController::GetAppConfig()
    {
        return config;
    }

    void AppController::LoadAppConfig()
    {
        if (configStorage_->Load() == std::nullopt)
        {
            // todo rework this
        } else
        {
            config = configStorage_->Load().value();
        }
    }

    bool AppController::SaveAppConfig() const
    {
        return configStorage_->Save(config);
    }
}
