/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Button
*/

#include "Button.hpp"

bool Button::isClicked(sf::RenderWindow &window, const sf::Event &event) const
{
    if ((event.type == sf::Event::MouseButtonPressed)
        && (event.mouseButton.button == sf::Mouse::Left)) {
        const sf::Vector2f mousePos = window.mapPixelToCoords(
            { event.mouseButton.x, event.mouseButton.y });
        if (buttonShape.getGlobalBounds().contains(mousePos)) {
            return true;
        }
    }
    return false;
}

Button::Button(float x, float y, float width, float height, const string &text,
    sf::Font &buttonFont) :
    _currentSize(TextSize::MEDIUM)
{
    buttonShape.setSize(sf::Vector2f(width, height));
    buttonShape.setOrigin(0.f, 0.f);
    buttonShape.setPosition(x, y);
    buttonShape.setFillColor(sf::Color::White);
    buttonShape.setOutlineColor(sf::Color::Black);
    buttonShape.setOutlineThickness(0.f);

    font = buttonFont;
    buttonText.setFont(font);
    buttonText.setString(text);
    buttonText.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::BUTTON_TEXT, _currentSize));
    buttonText.setFillColor(sf::Color::Black);

    recenterText();
}

void Button::updateTextSize(const TextSize size)
{
    _currentSize = size;
    buttonText.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::BUTTON_TEXT, _currentSize));
    recenterText();
}

void Button::recenterText()
{
    sf::FloatRect textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.left + textBounds.width / 2.0f,
        textBounds.top + textBounds.height / 2.0f);
    sf::FloatRect shapeBounds = buttonShape.getGlobalBounds();
    buttonText.setPosition(shapeBounds.left + shapeBounds.width / 2.f,
        shapeBounds.top + shapeBounds.height / 2.f);
}

void Button::draw(sf::RenderWindow &window)
{
    if (isMouseOver(window)) {
        buttonShape.setFillColor(hoverColor);
    } else {
        buttonShape.setFillColor(defaultColor);
    }

    window.draw(buttonShape);
    window.draw(buttonText);
}

bool Button::isMouseOver(sf::RenderWindow &window) const
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = buttonShape.getGlobalBounds();
    return bounds.contains(mousePos.x, mousePos.y);
}

void Button::setText(const string &text)
{
    buttonText.setString(text);
    recenterText();
}

bool Button::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed
        && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };
        if (buttonShape.getGlobalBounds().contains(
                static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y))) {
            if (onClick) {
                onClick();
            }
            return true;
        }
    }
    return false;
}

void Button::setPosition(float x, float y)
{
    buttonShape.setPosition(x, y);

    sf::FloatRect shapeBounds = buttonShape.getLocalBounds();
    buttonText.setPosition(
        x + shapeBounds.width / 2.f, y + shapeBounds.height / 2.f);
}

const string Button::getText() const { return buttonText.getString(); }

void Button::setBackgroundColor(const sf::Color &color)
{
    buttonShape.setFillColor(color);
}

void Button::setEnabled(bool enabled) { (void)enabled; }

bool Button::isEnabled() const { return true; }

sf::FloatRect Button::getGlobalBounds() const
{
    return buttonShape.getGlobalBounds();
}

void Button::setCharacterSize(unsigned int size)
{
    buttonText.setCharacterSize(size);
}
