/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Modal
*/

#pragma once

#include "../Button/Button.hpp"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>

using namespace std;

class Modal {
public:
    Modal(sf::Vector2f size, const sf::Font &font, vector<string> tabs = {});

    Modal(sf::Vector2f size, const sf::Font &font, const string &title);

    Modal(sf::Vector2f size, const sf::Font &font, bool hasTitle);

    void draw(sf::RenderWindow &window);
    bool handleEvent(const sf::Event &event);

    void setPosition(float x, float y);

    void setText(const string &text) { m_text.setString(text); }

    void addButton(const Button &button, function<void()> onClick);

    void show();

    void hide() { m_isVisible = false; }

    bool isVisible() const { return m_isVisible; }

    void addTab(const string &tabName);
    void addTab(
        const string &tabName, function<void(sf::RenderWindow &)> drawContent);
    void selectTab(size_t index);

    bool hasTabs() const { return !m_tabs.empty(); }

    const sf::RectangleShape &getBackground() const { return m_background; }

    const sf::RectangleShape &getContent() const { return m_content; }

    const sf::Text &getText() const { return m_text; }

    const vector<sf::RectangleShape> &getButtons() const { return m_buttons; }

    const vector<function<void()>> &getButtonCallbacks() const
    {
        return m_buttonCallbacks;
    }

    const vector<string> &getTabs() const { return m_tabs; }

    size_t getSelectedTab() const { return m_selectedTab; }

    void setBackground(const sf::RectangleShape &background)
    {
        m_background = background;
    }

    void setContent(const sf::RectangleShape &content) { m_content = content; }

    void setText(const sf::Text &text) { m_text = text; }

    void setButtons(const vector<sf::RectangleShape> &buttons)
    {
        m_buttons = buttons;
    }

    void setButtonCallbacks(const vector<function<void()>> &callbacks)
    {
        m_buttonCallbacks = callbacks;
    }

    void setTabs(const vector<string> &tabs) { m_tabs = tabs; }

    void setSelectedTab(size_t index);

    void setSize(const sf::Vector2f &size);

    void setTitle(const string &title);

    void setSaveCallback(function<void()> callback)
    {
        m_saveCallback = callback;
    }

    void setPendingChanges(bool hasChanges);

    bool hasPendingChanges() const { return m_hasChanges; }

    void initializeSaveButton();

    void addComponent(IComponent *component);

    void updateTextSizes(TextSize size);

private:
    bool m_isVisible;
    bool m_ignoreNextEvent = false;
    sf::RectangleShape m_background;
    sf::RectangleShape m_content;
    sf::Text m_text;
    sf::Font m_font;
    vector<sf::RectangleShape> m_buttons;
    vector<function<void()>> m_buttonCallbacks;

    vector<string> m_tabs;
    size_t m_selectedTab;

    sf::RectangleShape m_header;
    sf::Text m_title;

    vector<function<void(sf::RenderWindow &)>> m_tabContentDrawFunctions;
    void updateTabPositions();

    sf::RectangleShape m_shadow;

    sf::CircleShape m_topLeftCorner;
    sf::CircleShape m_topRightCorner;
    sf::CircleShape m_bottomLeftCorner;
    sf::CircleShape m_bottomRightCorner;

    vector<unique_ptr<Button>> m_tabButtons;
    unique_ptr<Button> m_saveButton;
    bool m_hasChanges = false;
    function<void()> m_saveCallback;

    vector<IComponent *> m_components;
};
