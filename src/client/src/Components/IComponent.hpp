/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** IComponent
*/

#pragma once

#include <SFML/Graphics.hpp>

class IComponent {
public:
    virtual void draw(sf::RenderWindow &window) = 0;
    virtual bool handleEvent(const sf::Event &event) = 0;
    virtual ~IComponent() = default;
};
