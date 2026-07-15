#pragma once
#include <chrono>

namespace stx
{
    // BASE KERNEL COOLDOWN FOR REQUEST
    constexpr std::chrono::seconds cooldown = std::chrono::seconds(3);

    class TimeController final
    {
    public:
        TimeController();
        ~TimeController() = default;

        [[nodiscard]] bool canMakeRequest() const;
        bool tryAcquireRequest();
    private:
        std::chrono::time_point<std::chrono::steady_clock> lastRequestTime_;
    };
}
