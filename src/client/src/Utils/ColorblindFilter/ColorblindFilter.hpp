/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Colorblind
*/

#pragma once
#include "ColorblindFilterException.hpp"
#include <SFML/Graphics.hpp>

class ColorblindFilter {
public:
    enum class Mode { PROTANOPIA, DEUTERANOPIA, TRITANOPIA };

    ColorblindFilter();
    ~ColorblindFilter();

    void prepareTexture(sf::RenderWindow &window, const sf::Drawable &scene);
    void draw(sf::RenderWindow &window);

    void setMode(Mode mode);
    Mode getCurrentMode() const;

private:
    sf::Shader m_colorblindShader;
    Mode m_currentMode;

    sf::RenderTexture m_renderTexture;
    sf::Sprite m_sceneSprite;
};
