#pragma once

#include "i_screen.h"
#include "models/app_config.h"

namespace screen {
    class ProfileScreen final : public IScreen {
    public:
        explicit ProfileScreen(AppConfig& config);
        void run() override;
        void printScreen() override;
    private:
        AppConfig& config_;
    };
}

enum class kPROFILE_MENU {
    kMinChoice = 1,
    kMaxChoice = 2,
    kChangeNickname = 1,
    kExit = 2
};