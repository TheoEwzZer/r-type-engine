/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Player
*/
#pragma once
#include "../Component.hpp"

class Player : public Component {
public:
    Player() = default;

    bool hasForce = false;
    bool isForceInFront = false;
    bool isForceAttached = false;
};
