/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Select
*/

#pragma once

#include "../TextStyle.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include <vector>

using namespace std;

class Select {
public:
    Select(const sf::Vector2f position, const sf::Vector2f size,
        const sf::Font &font, const string &title);

    virtual ~Select() = default;

    void addOption(const string &option);
    void draw(sf::RenderWindow &window) const;
    void handleEvent(const sf::Event &event, const sf::RenderWindow &window);
    void setCallback(const function<void(const string &)> &callback);
    void setSelectedOption(const string &option);
    string getSelectedOption() const;

    sf::Vector2f getPosition() const { return m_mainBox.getPosition(); }

    virtual void hide() { m_isVisible = false; }

    virtual void show() { m_isVisible = true; }

    virtual bool isVisible() const { return m_isVisible; }

    void updateTextSize(const TextSize size);
    void recenterText();
    void setTitle(const string &title);

    void setPosition(const sf::Vector2f position)
    {
        m_mainBox.setPosition(position);
    }

private:
    bool m_isVisible = true;
    sf::RectangleShape m_mainBox;
    sf::Text m_text;
    vector<string> m_options;
    string m_selectedOption;
    bool m_isOpen;
    function<void(const string &)> m_callback;
    sf::Font m_font;
    vector<sf::RectangleShape> m_optionBoxes;
    vector<sf::Text> m_optionTexts;
    TextSize m_currentTextSize;
    sf::Text m_title;

    void updateDisplay();
};
