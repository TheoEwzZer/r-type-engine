/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Collider
*/

#pragma once

#include "../Component.hpp"
#include <chrono>

class Collider : public Component {
public:
    Collider(const unsigned short width, const unsigned short height,
        const float scaleX, const float scaleY) :
        width(width),
        height(height), scaleX(scaleX), scaleY(scaleY)
    {
    }

    unsigned short width;
    unsigned short height;
    float scaleX;
    float scaleY;
};
