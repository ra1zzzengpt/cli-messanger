#pragma once

#include "i_screen.h"
#include "models/app_config.h"

namespace screen {
    class MainScreen final : public IScreen {
    public:
        explicit MainScreen(AppConfig& cfg);
        void run() override;
        void printScreen() override;
    private:
        AppConfig& config_;
    };
}

enum class kMAIN_MENU {
    kMinChoice = 1,
    kMaxChoice = 4,
    kChatScreen = 1,
    kProfileScreen = 2,
    kServerScreen = 3,
    kExit = 4
};