/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Button
*/

#pragma once

#include "../IComponent.hpp"
#include "../TextStyle.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

using namespace std;

class Button : public IComponent {
private:
    sf::RectangleShape buttonShape;
    sf::Text buttonText;
    sf::Font font;
    sf::Color defaultColor = sf::Color(70, 130, 180);
    sf::Color hoverColor = sf::Color(100, 149, 237);
    sf::Color activeColor = sf::Color(65, 105, 225);
    TextSize _currentSize;
    function<void()> onClick;

public:
    Button(float x, float y, float width, float height, const string &text,
        sf::Font &buttonFont);
    void draw(sf::RenderWindow &window);
    bool isMouseOver(sf::RenderWindow &window) const;
    bool isClicked(sf::RenderWindow &window, const sf::Event &event) const;
    bool handleEvent(const sf::Event &event) override;
    void setText(const string &text);
    const string getText() const;
    void setPosition(float x, float y);
    void setBackgroundColor(const sf::Color &color);
    void updateTextSize(const TextSize size);
    void recenterText();
    void setEnabled(bool enabled);
    bool isEnabled() const;
    sf::FloatRect getGlobalBounds() const;

    void setOnClick(function<void()> callback) { onClick = callback; }

    const sf::RectangleShape &getBackground() const { return buttonShape; }

    sf::Vector2f getPosition() const { return buttonShape.getPosition(); }

    sf::Vector2f getSize() const { return buttonShape.getSize(); }

    void setCharacterSize(unsigned int size);
};
