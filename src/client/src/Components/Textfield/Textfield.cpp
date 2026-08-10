/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** TextField
*/

#include "Textfield.hpp"

using enum TextStyle;

TextField::TextField(const sf::Vector2f position, const sf::Font &font,
    TextStyle style, const string &text, const sf::Color color) :
    _style(style),
    _currentSize(TextSize::MEDIUM)
{
    _font = font;
    _text.setFont(_font);
    _text.setFillColor(color);
    _text.setPosition(position);
    _text.setString(text);
    applyStyle();

    if (color == sf::Color::White && style != TextStyle::TITLE) {
        _text.setFillColor(sf::Color::Black);
    }
}

void TextField::updateTextSize(const TextSize size)
{
    _currentSize = size;
    _text.setCharacterSize(TextSizeConfig::getSize(_style, _currentSize));
}

void TextField::applyStyle()
{
    _text.setCharacterSize(TextSizeConfig::getSize(_style, _currentSize));
    switch (_style) {
        case TITLE:
            _text.setCharacterSize(50);
            _text.setStyle(sf::Text::Bold);
            break;
        case BUTTON_TEXT:
        case SETTINGS_LABEL:
            _text.setCharacterSize(20);
            _text.setStyle(sf::Text::Regular);
            break;
        case NORMAL_TEXT:
        case SUBTITLE:
            _text.setCharacterSize(24);
            _text.setStyle(sf::Text::Regular);
            break;
        case WARNING_TEXT:
            _text.setCharacterSize(16);
            _text.setStyle(sf::Text::Bold);
            break;
        case TAB_TITLE:
            _text.setCharacterSize(22);
            _text.setStyle(sf::Text::Bold);
            break;
        default:
            _text.setCharacterSize(20);
            _text.setStyle(sf::Text::Regular);
            break;
    }
}

void TextField::draw(sf::RenderWindow &window) { window.draw(_text); }

bool TextField::handleEvent(const sf::Event &event)
{
    (void)event;
    return false;
}
