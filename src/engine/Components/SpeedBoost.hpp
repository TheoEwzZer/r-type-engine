#pragma once
#include "../Component.hpp"
#include <chrono>

namespace rtype {
class SpeedBoost : public Component {
public:
    SpeedBoost(float duration = 5.0f, float speedMultiplier = 2.0f) :
        duration(duration), speedMultiplier(speedMultiplier), isActive(false)
    {
    }

    float duration;
    float speedMultiplier;
    bool isActive;
    chrono::steady_clock::time_point activationTime;
};
}
