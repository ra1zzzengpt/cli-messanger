#pragma once
#include "models/app_config.h"

class Application final {
public:
    void Run();
private:
    bool LoadConfig();

private:
    AppConfig config;
};