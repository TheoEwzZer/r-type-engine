/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Animation
*/

#pragma once

#include "../Component.hpp"
#include <chrono>

class Animation : public Component {
public:
    Animation(const unsigned short animationSpeed,
        const unsigned short animationLength,
        const unsigned short animationLengthDeath,
        rtype::GameplayAsset asset) :
        animationSpeed(animationSpeed),
        animationLength(animationLength),
        animationLengthDeath(animationLengthDeath), asset(asset)
    {
    }

    int currentFrame = 0;
    chrono::steady_clock::time_point lastFrameUpdate
        = chrono::steady_clock::now();
    unsigned short animationSpeed;
    unsigned short animationLength;
    unsigned short animationLengthDeath;
    rtype::GameplayAsset asset;
};
