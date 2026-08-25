#ifndef I_TIME_CONTROLLER_HPP
#define I_TIME_CONTROLLER_HPP
#include <chrono>

namespace stx
{
    // BASE KERNEL COOLDOWN FOR REQUEST
    constexpr std::chrono::seconds cooldown = std::chrono::seconds(3);

    class ITimeController
    {
    public:
        virtual ~ITimeController() = default;

        [[nodiscard]] virtual bool canMakeRequest() const = 0;
        virtual bool tryAcquireRequest() = 0;
    };
}
#endif