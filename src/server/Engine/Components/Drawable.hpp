/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Drawable
*/

#pragma once

#include "../Component.hpp"
#include "Protocol.hpp"
#include <chrono>

class Drawable : public Component {
public:
    explicit Drawable(const rtype::Sprite &sprite) : sprite(sprite) {}

    rtype::Sprite sprite;
};
