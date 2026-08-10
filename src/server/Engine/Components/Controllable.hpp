/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Controllable
*/

#pragma once

#include "../Component.hpp"

class Controllable : public Component {
public:
    Controllable(const bool up, const bool down, const bool left,
        const bool right, const int speed) :
        up(up),
        down(down), left(left), right(right), speed(speed)
    {
    }

    Controllable() = default;

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool prevUp = false;
    bool prevDown = false;
    bool prevLeft = false;
    bool prevRight = false;
    int speed = 5;
};
