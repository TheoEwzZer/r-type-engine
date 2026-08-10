/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Textfield
*/

#pragma once

#include "../IComponent.hpp"
#include "../TextStyle.hpp"
#include <SFML/Graphics.hpp>

using namespace std;

class TextInput : public IComponent {
public:
    TextInput(sf::Vector2f position, sf::Vector2f size, const sf::Font &font);

private:
    sf::RectangleShape background;
    sf::Text text;
    sf::Font font;
    bool isActive;
    size_t maxLength;
    TextSize currentSize;
    sf::Vector2f position;

public:
    void draw(sf::RenderWindow &window) override;
    bool handleEvent(const sf::Event &event) override;
    string getText() const;
    void setMaxLength(size_t length);
    void setPosition(sf::Vector2f newPosition);
    void updateTextSize(const TextSize size);
    void recenterText();
};
