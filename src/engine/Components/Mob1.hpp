/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Mob1
*/

#pragma once

#include "../Component.hpp"
#include <random>

class Mob1 : public Component {
public:
    Mob1()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(2.0f, 5.0f);
        shootCooldown = dis(gen);
    }

    float shootCooldown;
    float lastShootTime = 0.0f;
};
