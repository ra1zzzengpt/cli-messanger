#pragma once

#include "i_screen.h"
#include "models/app_config.h"

namespace screen {
    class ServerScreen final : public IScreen {
    public:
        explicit ServerScreen(AppConfig& cfg);
        void run() override;
        void printScreen() override;

    private:
        AppConfig& config_;
    };
}

enum class kSERVER_MENU {
    kMinChoice = 1,
    kMaxChoice = 3,
    kChangeIp = 1,
    kChangePort = 2,
    kExit = 3
};
