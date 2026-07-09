#include <utils/time/time_controller.hpp>

namespace stx
{
    TimeController::TimeController() : lastRequestTime_(std::chrono::steady_clock::now() - cooldown) {}

    bool TimeController::askForRequest(const std::chrono::time_point<std::chrono::steady_clock> compare)
    {
        if (std::chrono::duration_cast<std::chrono::seconds>(lastRequestTime_ - compare) < cooldown)
        {
            lastRequestTime_ = compare;
            return true;
        }
        return false;
    }
}
