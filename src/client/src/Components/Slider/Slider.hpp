/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Modal
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;

class Slider {
public:
    Slider(sf::Vector2f position, sf::Vector2f size);
    void draw(sf::RenderWindow &window) const;
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void setPosition(float x, float y);
    float getValue() const;
    void setValue(float value);

    float getMinValue() const { return minValue; }

    float getMaxValue() const { return maxValue; }

    bool getIsDragging() const { return isDragging; }

    void setMinValue(float min) { minValue = min; }

    void setMaxValue(float max) { maxValue = max; }

    void setIsDragging(bool dragging) { isDragging = dragging; }

    void setSliderColor(const sf::Color &color) { slider.setFillColor(color); }

    void setHandleColor(const sf::Color &color) { handle.setFillColor(color); }

    void updateHandlePosition();

    void setRange(float minVal, float maxVal);

    void setCallback(const function<void(float)> &callback);

private:
    sf::RectangleShape slider;
    sf::RectangleShape handle;
    bool isDragging;
    float minValue;
    float maxValue;
    float currentValue;
    function<void(float)> _callback;
};
