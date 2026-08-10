/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Boss1
*/

#pragma once

#include "../Component.hpp"
#include <random>

class Boss1 : public Component {
public:
    Boss1()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(1.0f, 2.0f);
        shootCooldown = dis(gen);
    }

    float shootCooldown;
    float lastShootTime = 0.0f;
};
