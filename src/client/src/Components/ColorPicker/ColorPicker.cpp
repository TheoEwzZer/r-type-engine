/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** ColorPicker
*/

#include "ColorPicker.hpp"

vector<sf::Color> ColorPicker::defaultColors = { sf::Color::White,
    sf::Color::Black, sf::Color::Red, sf::Color::Green, sf::Color::Blue,
    sf::Color::Yellow, sf::Color::Magenta, sf::Color::Cyan,
    sf::Color(128, 128, 128), sf::Color(192, 192, 192), sf::Color(128, 0, 0),
    sf::Color(0, 128, 0), sf::Color(0, 0, 128), sf::Color(128, 128, 0),
    sf::Color(128, 0, 128), sf::Color(0, 128, 128) };

ColorPicker::ColorPicker(sf::Vector2f position) :
    currentColor(sf::Color::Black)
{
    colorPreview.setSize({ 50, 50 });
    colorPreview.setPosition(position);
    colorPreview.setFillColor(currentColor);
    colorPreview.setOutlineThickness(2);
    colorPreview.setOutlineColor(sf::Color::White);

    paletteBackground.setSize({ 200, 100 });
    paletteBackground.setPosition(position.x + 60, position.y);
    paletteBackground.setFillColor(sf::Color(50, 50, 50));
    paletteBackground.setOutlineThickness(2);
    paletteBackground.setOutlineColor(sf::Color::White);

    createColorPalette();
}

void ColorPicker::createColorPalette()
{
    const float swatchSize = 25.0f;
    const float startX = paletteBackground.getPosition().x + 10;
    const float startY = paletteBackground.getPosition().y + 10;
    const int colorsPerRow = 6;

    for (size_t i = 0; i < defaultColors.size(); ++i) {
        sf::RectangleShape swatch({ swatchSize, swatchSize });
        float x = startX + (i % colorsPerRow) * (swatchSize + 5);
        float y = startY + (i / colorsPerRow) * (swatchSize + 5);
        swatch.setPosition(x, y);
        swatch.setFillColor(defaultColors[i]);
        colorPalette.push_back({ swatch, defaultColors[i] });
    }
}

void ColorPicker::draw(sf::RenderWindow &window)
{
    window.draw(paletteBackground);
    window.draw(colorPreview);
    for (const auto &swatch : colorPalette) {
        window.draw(swatch.first);
    }
}

bool ColorPicker::handleEvent(
    const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (!paletteBackground.getGlobalBounds().contains(
                static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y))) {
            return false;
        }

        for (const auto &swatch : colorPalette) {
            if (swatch.first.getGlobalBounds().contains(
                    mousePos.x, mousePos.y)) {
                currentColor = swatch.second;
                colorPreview.setFillColor(currentColor);
                if (onColorChange) {
                    onColorChange(currentColor);
                }
                return true;
            }
        }
    }
    return false;
}

void ColorPicker::setCallback(const function<void(sf::Color)> &callback)
{
    onColorChange = callback;
}

sf::Color ColorPicker::getCurrentColor() const { return currentColor; }
