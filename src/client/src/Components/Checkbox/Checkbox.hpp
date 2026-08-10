/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Checkbox
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;

class Checkbox {
public:
    Checkbox(sf::Vector2f position, float size, const sf::Font &font,
        const string &label = "");

    void draw(sf::RenderWindow &window);
    bool handleEvent(const sf::Event &event, const sf::RenderWindow &window);

    void setChecked(bool checked);

    bool isChecked() const { return m_checked; }

    void setCallback(function<void(bool)> callback) { m_onChange = callback; }

    void setPosition(const sf::Vector2f position);

private:
    sf::RectangleShape m_box;
    sf::RectangleShape m_checkmark;
    sf::Text m_label;
    bool m_checked;
    function<void(bool)> m_onChange;
};
