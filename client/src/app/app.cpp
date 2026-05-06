#include "app.h"

#include "screens/main_screen.h"
#include "utils/console/console.h"
#include "utils/files/file_utils.h"

#include <nlohmann/json.hpp>

void Application::Run() {
    if (!utils::LoadConfig(config)) {
        io::print("[Error]: Cannot load config", io::COLOR::RED);
        return;
    }

    screen::MainScreen mainScreen(config);
    mainScreen.run();
}
