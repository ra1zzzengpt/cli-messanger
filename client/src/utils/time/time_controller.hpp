#pragma once
#include <chrono>
#include "i_time_controller.hpp"

namespace stx
{
    class TimeController final : public ITimeController
    {
    public:
        TimeController();
        ~TimeController() override = default;

        [[nodiscard]] bool canMakeRequest() const override;
        bool tryAcquireRequest() override;
    private:
        std::chrono::time_point<std::chrono::steady_clock> lastRequestTime_;
    };
}
