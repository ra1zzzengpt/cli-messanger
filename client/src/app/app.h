#pragma once
#include "models/app_config.h"

class Application final {
public:
    void Run();
private:
    // -- FUNCTIONS --
    bool LoadConfig();

    // -- VAR --
    AppConfig config;
};