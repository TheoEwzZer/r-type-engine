/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Health
*/

#pragma once

#include "../Component.hpp"
#include <chrono>

class Health : public Component {
public:
    explicit Health(const int lives) : lives(lives) {}

    int lives;
};
