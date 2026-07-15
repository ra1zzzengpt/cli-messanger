#include <utils/time/time_controller.hpp>

namespace stx
{
    TimeController::TimeController() : lastRequestTime_(std::chrono::steady_clock::now() - cooldown) {}

    bool TimeController::canMakeRequest() const
    {
        return std::chrono::steady_clock::now() - lastRequestTime_ >= cooldown;
    }

    bool TimeController::tryAcquireRequest()
    {
        if (!canMakeRequest()) return false;
        lastRequestTime_ = std::chrono::steady_clock::now();
        return true;
    }
}
