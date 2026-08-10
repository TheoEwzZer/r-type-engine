/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Force
*/

#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"
#include <random>

class Force : public Component {
public:
    Force()
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dis(1.0f, 2.0f);
        shootCooldown = dis(gen);
    }

    bool attached = false;
    bool isForceInFront = false;
    int playerId = -1;
    unsigned int power = 10;
    unsigned level = 1;
    vector<rtype::ecs::Entity> hitObstacles;
    chrono::steady_clock::time_point getTime;
    float shootCooldown;
    float lastShootTime = 0.0f;
};
