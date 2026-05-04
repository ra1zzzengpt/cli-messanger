#pragma once

#include "i_screen.h"
#include "models/app_config.h"

namespace screen {
    class RegisterScreen final : public IScreen {
    public:
        explicit RegisterScreen(AppConfig& cfg);
        void run() override;
        void printScreen() override;

    private:
        AppConfig& config_;
    };
}
