#pragma once

namespace screen {
    class IScreen {
    public:
        virtual ~IScreen() = default;
        virtual void run() = 0;
        virtual void printScreen() = 0;
    };
}