/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** ColorPicker
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;

class ColorPicker {
public:
    explicit ColorPicker(sf::Vector2f position);
    void draw(sf::RenderWindow &window);
    bool handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void setCallback(const function<void(sf::Color)> &callback);
    sf::Color getCurrentColor() const;

private:
    void createColorPalette();
    void updateColorPreview();

    sf::RectangleShape colorPreview;
    vector<pair<sf::RectangleShape, sf::Color>> colorPalette;
    sf::Color currentColor;
    function<void(sf::Color)> onColorChange;
    static vector<sf::Color> defaultColors;
    sf::RectangleShape paletteBackground;
};
