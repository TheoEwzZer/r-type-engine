/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Position
*/

#pragma once

#include "../Component.hpp"
#include <chrono>

class Position : public Component {
public:
    Position(const int x, const int y) : x(x), y(y) {}

    int x;
    int y;
};
