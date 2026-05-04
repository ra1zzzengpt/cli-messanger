#pragma once

#include "i_screen.h"
#include "models/app_config.h"

namespace screen {
    class ChatsScreen final : public IScreen {
    public:
        explicit ChatsScreen(AppConfig& cfg);
        void run() override;
        void printScreen() override;

    private:
        AppConfig& config_;
    };
}

enum class kCHATS_MENU {
    kMinChoice = 1,
    kMaxChoice = 3,
    kSelectChat = 1,
    kAddChat = 2,
    kExit = 3
};
