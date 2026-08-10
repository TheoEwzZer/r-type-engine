/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Mob2
*/

#pragma once

#include "../Component.hpp"
#include <random>

class Mob2 : public Component {
public:
    Mob2()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(2.0f, 5.0f);
        shootCooldown = dis(gen);
    }

    float shootCooldown;
    float lastShootTime = 0.0f;
};
