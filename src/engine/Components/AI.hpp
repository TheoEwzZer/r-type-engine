/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** AI
*/
#pragma once
#include "../Component.hpp"
#include <random>

class AI : public Component {
public:
    AI()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(1.0f, 2.0f);
        shootCooldown = dis(gen);
    }

    float shootCooldown;
    float lastShootTime = 0.0f;
};
