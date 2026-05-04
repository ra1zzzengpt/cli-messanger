#include "register_screen.h"

#include "utils/console/console.h"

namespace screen {
    RegisterScreen::RegisterScreen(AppConfig& cfg) : config_(cfg) {}

    void RegisterScreen::run() {
        printScreen();

        io::print("Server");
        // todo input server data

        io::print("Profile");
        // todo input profile data
    }

    void RegisterScreen::printScreen() {
        io::print("Register Screen");
        io::print("Registration order:");
        io::print("1 - Server");
        io::print("2 - Profile");
    }
}
