/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Projectile
*/

#pragma once

#include "../Component.hpp"
#include "../Entity.hpp"
#include <vector>

enum class ProjectileOrigin { PLAYER, ENEMY, BOSS1 };

class Projectile : public Component {
public:
    Projectile(const int speed, const bool isCharged, const float dirX,
        const float dirY, const ProjectileOrigin origin,
        const bool playerHasForce) :
        speed(speed),
        isCharged(isCharged), dirX(dirX), dirY(dirY), origin(origin)
    {
        if ((origin == ProjectileOrigin::PLAYER) && playerHasForce) {
            power *= 2;
        }
    }

    Projectile(const int speed, const bool isCharged, const float dirX,
        const float dirY, const ProjectileOrigin origin) :
        Projectile(speed, isCharged, dirX, dirY, origin, false)
    {
    }

    int speed;
    int power = 20;
    bool isCharged;
    float dirX;
    float dirY;
    ProjectileOrigin origin;
    vector<rtype::ecs::Entity> hitObstacles;
};
