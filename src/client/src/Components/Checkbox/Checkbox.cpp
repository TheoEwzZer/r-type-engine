/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Checkbox
*/

#include "Checkbox.hpp"

Checkbox::Checkbox(sf::Vector2f position, float size, const sf::Font &font,
    const string &label) :
    m_checked(false)
{
    m_box.setSize(sf::Vector2f(size, size));
    m_box.setPosition(position);
    m_box.setFillColor(sf::Color::White);
    m_box.setOutlineColor(sf::Color::Black);
    m_box.setOutlineThickness(2);

    float checkmarkPadding = size * 0.2f;
    m_checkmark.setSize(sf::Vector2f(
        size - (checkmarkPadding * 2), size - (checkmarkPadding * 2)));
    m_checkmark.setPosition(
        position.x + checkmarkPadding, position.y + checkmarkPadding);
    m_checkmark.setFillColor(sf::Color::Black);

    if (!label.empty()) {
        m_label.setFont(font);
        m_label.setString(label);
        m_label.setCharacterSize(size * 0.8f);
        m_label.setFillColor(sf::Color::Black);
        m_label.setPosition(position.x + size + 10, position.y);
    }
}

void Checkbox::draw(sf::RenderWindow &window)
{
    window.draw(m_box);
    if (m_checked) {
        window.draw(m_checkmark);
    }
    if (!m_label.getString().isEmpty()) {
        window.draw(m_label);
    }
}

bool Checkbox::handleEvent(
    const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (m_box.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            m_checked = !m_checked;
            if (m_onChange) {
                m_onChange(m_checked);
            }
            return true;
        }
    }
    return false;
}

void Checkbox::setChecked(bool checked) { m_checked = checked; }

void Checkbox::setPosition(const sf::Vector2f position)
{
    float size = m_box.getSize().x;
    float checkmarkPadding = size * 0.2f;

    m_box.setPosition(position);
    m_checkmark.setPosition(
        position.x + checkmarkPadding, position.y + checkmarkPadding);

    if (!m_label.getString().isEmpty()) {
        m_label.setPosition(position.x + size + 10, position.y);
    }
}
