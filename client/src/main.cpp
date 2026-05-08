#include "app/app_controller.h"
#include "screens/main_screen.h"
#include "utils/files/config_storage/config_storage.h"
#include "api/message_api/httpapi/http_message_api.h"
#include <curl/curl.h>

#include "utils/console/console.h"

namespace
{
    // YOU CAN CHANGE IT ON YOUR CONFIG FILEPATH
    constexpr auto kConfigPath = "../client/assets/save/save.json";
}

int main()
{
    if (const CURLcode init_code = curl_global_init(CURL_GLOBAL_ALL); init_code != CURLE_OK)
    {
        io::print("[Fatal error]: curl init failed: " + std::to_string(init_code));
        return 1;
    }
    app::AppController controller(std::make_unique<api::HttpMessageApi>(),std::make_unique<utils::ConfigStorage>(kConfigPath));

    screen::MainScreen mainMenu(controller);
    mainMenu.run();

    return 0;
}