/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Textfield
*/

#pragma once

#include "../IComponent.hpp"
#include "../TextStyle.hpp"

using namespace std;

class TextField : public IComponent {

private:
    sf::Text _text;
    sf::Font _font;
    TextStyle _style;
    float _xRatio;
    float _yRatio;
    TextSize _currentSize;

public:
    TextField(const sf::Vector2f position, const sf::Font &font,
        TextStyle style, const string &text,
        const sf::Color color = sf::Color::White);

    void draw(sf::RenderWindow &window) override;
    bool handleEvent(const sf::Event &event) override;

    void applyStyle();
    void updateTextSize(const TextSize size);

    void setText(const string &text) { _text.setString(text); }

    string getText() const { return _text.getString(); }

    float getXRatio() const { return _xRatio; }

    float getYRatio() const { return _yRatio; }

    void setXRatio(float xRatio) { _xRatio = xRatio; }

    void setYRatio(float yRatio) { _yRatio = yRatio; }
};
