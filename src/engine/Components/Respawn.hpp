/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Respawn
*/

#pragma once

#include "../Component.hpp"
#include <chrono>

class Respawn : public Component {
public:
    explicit Respawn(const chrono::steady_clock::time_point respawnTime) :
        respawnTime(respawnTime)
    {
    }

    chrono::steady_clock::time_point respawnTime;
};
