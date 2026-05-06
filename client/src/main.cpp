#include "app/app_controller.h"
#include "screens/main_screen.h"
#include "utils/files/config_storage/config_storage.h"
#include "api/message_api/fakeapi/fake_message_api.h"
namespace
{
    // YOU CAN CHANGE IT ON YOUR CONFIG FILEPATH
    constexpr auto kConfigPath = "client/assets/save/save.json";
}

int main()
{

    AppController controller(std::make_unique<api::FakeMessageApi>(),std::make_unique<utils::ConfigStorage>(kConfigPath));

    screen::MainScreen mainMenu(controller);
    mainMenu.Run();

    return 0;
}