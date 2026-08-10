/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** TextInput
*/

#include "Textinput.hpp"

TextInput::TextInput(
    sf::Vector2f position, sf::Vector2f size, const sf::Font &fontValue) :
    currentSize(TextSize::MEDIUM)
{
    background.setPosition(position);
    background.setSize(size);
    background.setFillColor(sf::Color::White);
    background.setOutlineColor(sf::Color::Black);
    background.setOutlineThickness(2);

    text.setFont(fontValue);
    text.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::NORMAL_TEXT, currentSize));
    text.setFillColor(sf::Color::Black);
    text.setString("");
    text.setPosition(position.x + 5, position.y + size.y / 4);

    isActive = false;
    maxLength = 100;
}

bool TextInput::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };
        isActive
            = background.getGlobalBounds().contains(mousePos.x, mousePos.y);
        return isActive;
    }

    if (event.type == sf::Event::TextEntered && isActive) {
        if (event.text.unicode == '\b') {
            string str = text.getString();
            if (!str.empty()) {
                str.pop_back();
                text.setString(str);
            }
        } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
            string str = text.getString();
            if (str.length() < maxLength) {
                str += static_cast<char>(event.text.unicode);
                text.setString(str);
            }
        }
        return true;
    }
    return false;
}

void TextInput::draw(sf::RenderWindow &window)
{
    window.draw(background);
    window.draw(text);
}

string TextInput::getText() const { return text.getString(); }

void TextInput::setMaxLength(size_t length) { maxLength = length; }

void TextInput::setPosition(sf::Vector2f newPosition)
{
    position = newPosition;
    background.setPosition(position);
    recenterText();
}

void TextInput::updateTextSize(const TextSize size)
{
    currentSize = size;
    text.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::NORMAL_TEXT, currentSize));
    recenterText();
}

void TextInput::recenterText()
{
    text.setPosition(background.getPosition().x + 5,
        background.getPosition().y + background.getSize().y / 4);
}
