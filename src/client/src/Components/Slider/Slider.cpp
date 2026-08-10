/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Slider
*/

#include "Slider.hpp"
#include <functional>

Slider::Slider(sf::Vector2f position, sf::Vector2f size) :
    isDragging(false), minValue(0.f), maxValue(100.f), currentValue(50.f),
    _callback(nullptr)
{
    slider.setPosition(position);
    slider.setSize(size);
    slider.setFillColor(sf::Color(150, 150, 150));

    handle.setSize(sf::Vector2f(20.f, size.y + 10.f));
    handle.setFillColor(sf::Color::White);
    updateHandlePosition();
}

void Slider::draw(sf::RenderWindow &window) const
{
    window.draw(slider);
    window.draw(handle);
}

void Slider::handleEvent(
    const sf::Event &event, const sf::RenderWindow &window)
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect handleBounds = handle.getGlobalBounds();
    sf::FloatRect sliderBounds = slider.getGlobalBounds();

    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        if (sliderBounds.contains(
                static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))
            || handleBounds.contains(static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y))) {
            isDragging = true;
            float newX
                = static_cast<float>(mousePos.x) - handle.getSize().x / 2;
            float minX = slider.getPosition().x;
            float maxX = slider.getPosition().x + slider.getSize().x
                - handle.getSize().x;
            newX = max(minX, min(newX, maxX));
            handle.setPosition(newX, handle.getPosition().y);

            float percentage = (newX - minX) / (maxX - minX);
            currentValue = minValue + percentage * (maxValue - minValue);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased
        && event.mouseButton.button == sf::Mouse::Left) {
        isDragging = false;
    }

    if (isDragging) {
        float newX = static_cast<float>(mousePos.x) - handle.getSize().x / 2;
        float minX = slider.getPosition().x;
        float maxX
            = slider.getPosition().x + slider.getSize().x - handle.getSize().x;
        newX = max(minX, min(newX, maxX));

        handle.setPosition(newX, handle.getPosition().y);

        float percentage = (newX - minX) / (maxX - minX);
        currentValue = minValue + percentage * (maxValue - minValue);

        if (_callback) {
            _callback(currentValue);
        }
    }
}

void Slider::setPosition(float x, float y)
{
    slider.setPosition(x, y);
    updateHandlePosition();
}

float Slider::getValue() const { return currentValue; }

void Slider::setValue(float value)
{
    currentValue = max(minValue, min(value, maxValue));
    float percentage = (currentValue - minValue) / (maxValue - minValue);
    float handleX = slider.getPosition().x
        + percentage * (slider.getSize().x - handle.getSize().x);
    handle.setPosition(handleX, handle.getPosition().y);
}

void Slider::setRange(float minVal, float maxVal)
{
    minValue = minVal;
    maxValue = maxVal;
    setValue(currentValue);
}

void Slider::updateHandlePosition()
{
    float percentage = (currentValue - minValue) / (maxValue - minValue);
    float handleX = slider.getPosition().x
        + percentage * (slider.getSize().x - handle.getSize().x);
    handle.setPosition(handleX, slider.getPosition().y - 5.f);
}

void Slider::setCallback(const function<void(float)> &callback)
{
    _callback = callback;
}
