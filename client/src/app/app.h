#pragma once
#include "models/app_config.h"

class Application final {
public:
    void Run();
private:
    // -- VAR --
    AppConfig config;
};