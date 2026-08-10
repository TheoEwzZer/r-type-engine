/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Modal
*/

#include "Modal.hpp"
#include "../Button/Button.hpp"

Modal::Modal(sf::Vector2f size, const sf::Font &font, vector<string> tabs) :
    m_tabs(tabs), m_selectedTab(0)
{
    m_isVisible = false;
    m_ignoreNextEvent = false;
    m_font = font;

    m_background.setSize(size);
    m_background.setFillColor(sf::Color(245, 245, 245));
    m_background.setOutlineColor(sf::Color::Transparent);
    m_background.setOutlineThickness(0.f);

    m_header.setSize(sf::Vector2f(size.x, 50));
    m_header.setFillColor(sf::Color(45, 45, 45));

    m_title.setFont(m_font);
    m_title.setCharacterSize(24);
    m_title.setFillColor(sf::Color::White);
    m_title.setString("Settings");
    m_title.setPosition(0, 0);

    m_content.setSize(sf::Vector2f(size.x, size.y - 50));
    m_content.setFillColor(sf::Color(200, 200, 200));
    m_content.setPosition(0, 50);

    m_shadow.setSize(sf::Vector2f(size.x + 15, size.y + 15));
    m_shadow.setFillColor(sf::Color(0, 0, 0, 80));
    m_shadow.setPosition(m_background.getPosition().x - 7.5f,
        m_background.getPosition().y - 7.5f);

    float cornerRadius = 15.f;
    m_topLeftCorner.setRadius(cornerRadius);
    m_topLeftCorner.setFillColor(m_background.getFillColor());
    m_topLeftCorner.setPosition(
        m_background.getPosition().x, m_background.getPosition().y);

    m_topRightCorner.setRadius(cornerRadius);
    m_topRightCorner.setFillColor(m_background.getFillColor());
    m_topRightCorner.setPosition(
        m_background.getPosition().x + size.x - cornerRadius * 2,
        m_background.getPosition().y);

    m_bottomLeftCorner.setRadius(cornerRadius);
    m_bottomLeftCorner.setFillColor(m_background.getFillColor());
    m_bottomLeftCorner.setPosition(m_background.getPosition().x,
        m_background.getPosition().y + size.y - cornerRadius * 2);

    m_bottomRightCorner.setRadius(cornerRadius);
    m_bottomRightCorner.setFillColor(m_background.getFillColor());
    m_bottomRightCorner.setPosition(
        m_background.getPosition().x + size.x - cornerRadius * 2,
        m_background.getPosition().y + size.y - cornerRadius * 2);

    updateTabPositions();
}

Modal::Modal(sf::Vector2f size, const sf::Font &font, const string &title) :
    Modal(size, font, vector<string>())
{
    m_title.setString(title);
}

Modal::Modal(sf::Vector2f size, const sf::Font &font, bool hasTitle) :
    Modal(size, font, vector<string>())
{
    if (!hasTitle) {
        m_header.setSize(sf::Vector2f(0, 0));
        m_content.setPosition(m_content.getPosition().x, 0);
        m_content.setSize(sf::Vector2f(size.x, size.y));
        m_title.setString("");
        m_title.setFillColor(sf::Color::Transparent);
    }
}

void Modal::draw(sf::RenderWindow &window)
{
    if (!m_isVisible) {
        return;
    }

    window.draw(m_shadow);
    window.draw(m_background);
    window.draw(m_content);
    window.draw(m_header);

    for (size_t i = 0; i < m_tabButtons.size(); ++i) {
        m_tabButtons[i]->draw(window);
    }

    window.draw(m_title);

    if (m_selectedTab < m_tabContentDrawFunctions.size()) {
        m_tabContentDrawFunctions[m_selectedTab](window);
    }

    if (m_saveButton && m_hasChanges) {
        m_saveButton->setCharacterSize(14);
        m_saveButton->draw(window);
    }

    for (auto *component : m_components) {
        component->draw(window);
    }
}

bool Modal::handleEvent(const sf::Event &event)
{
    if (!m_isVisible) {
        return false;
    }
    if (m_ignoreNextEvent) {
        m_ignoreNextEvent = false;
        return true;
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };

        for (size_t i = 0; i < m_tabButtons.size(); ++i) {
            sf::FloatRect buttonBounds
                = m_tabButtons[i]->getBackground().getGlobalBounds();

            if (buttonBounds.contains(mousePos.x, mousePos.y)) {
                selectTab(i);
                return true;
            }
        }

        if (!m_background.getGlobalBounds().contains(mousePos.x, mousePos.y)
            && !m_header.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            hide();
            return true;
        }

        for (auto &button : m_buttons) {
            if (button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                auto it = find_if(m_buttons.begin(), m_buttons.end(),
                    [&button](const sf::RectangleShape &b) {
                        return b.getPosition() == button.getPosition();
                    });
                if (it != m_buttons.end()) {
                    size_t index = distance(m_buttons.begin(), it);
                    m_buttonCallbacks[index]();
                }
                return true;
            }
        }

        if (m_saveButton
            && m_saveButton->getGlobalBounds().contains(
                mousePos.x, mousePos.y)) {
            if (m_saveCallback) {
                m_saveCallback();
            }
            setPendingChanges(false);
            return true;
        }
    }
    return false;
}

void Modal::initializeSaveButton()
{
    m_saveButton = make_unique<Button>(
        m_background.getPosition().x + m_background.getSize().x - 120,
        m_background.getPosition().y + 5, 100, 40, "Save", m_font);
}

void Modal::setPendingChanges(bool hasChanges) { m_hasChanges = hasChanges; }

void Modal::setPosition(float x, float y)
{
    m_background.setPosition(x, y);
    m_header.setPosition(x, y);
    m_content.setPosition(x, y + m_header.getSize().y);
    m_title.setPosition(x + 20, y + 10);

    m_shadow.setPosition(x - 7.5f, y - 7.5f);

    float cornerRadius = 15.f;
    m_topLeftCorner.setPosition(x, y);
    m_topRightCorner.setPosition(
        x + m_background.getSize().x - cornerRadius * 2, y);
    m_bottomLeftCorner.setPosition(
        x, y + m_background.getSize().y - cornerRadius * 2);
    m_bottomRightCorner.setPosition(
        x + m_background.getSize().x - cornerRadius * 2,
        y + m_background.getSize().y - cornerRadius * 2);

    if (m_saveButton) {
        m_saveButton->setPosition(x + m_background.getSize().x - 120, y + 5);
    }

    updateTabPositions();
}

void Modal::addButton(const Button &button, function<void()> onClick)
{
    m_buttons.push_back(button.getBackground());
    m_buttonCallbacks.push_back(onClick);
}

void Modal::addTab(
    const string &tabName, function<void(sf::RenderWindow &)> drawContent)
{
    m_tabs.push_back(tabName);

    const float TAB_WIDTH = 150.f;
    const float TAB_HEIGHT = 30.f;
    const float TAB_SPACING = 20.f;
    float startX = m_background.getPosition().x + 50.f
        + (TAB_WIDTH + TAB_SPACING) * m_tabs.size();
    float y = m_background.getPosition().y + m_header.getSize().y / 2
        - TAB_HEIGHT / 2;

    auto tabButton = make_unique<Button>(
        startX, y, TAB_WIDTH, TAB_HEIGHT, tabName, m_font);
    tabButton->setCharacterSize(18);

    m_tabButtons.push_back(move(tabButton));
    m_tabContentDrawFunctions.push_back(drawContent);

    updateTabPositions();
}

void Modal::updateTabPositions()
{
    const float TAB_WIDTH = 150.f;
    const float TAB_HEIGHT = 30.f;
    const float TAB_PADDING = 15.f;

    float titleRightEdge = m_title.getPosition().x
        + m_title.getLocalBounds().width + TAB_PADDING;

    float startX = titleRightEdge;
    float y = m_background.getPosition().y + m_header.getSize().y / 2
        - TAB_HEIGHT / 2;

    for (size_t i = 0; i < m_tabButtons.size(); ++i) {
        m_tabButtons[i]->setPosition(
            startX + (TAB_WIDTH + TAB_PADDING) * i, y);
    }
}

void Modal::selectTab(size_t index)
{
    if (index < m_tabs.size()) {
        m_selectedTab = index;
        for (size_t i = 0; i < m_tabButtons.size(); ++i) {
            if (i == index) {
                m_tabButtons[i]->setBackgroundColor(sf::Color(70, 70, 70));
            } else {
                m_tabButtons[i]->setBackgroundColor(sf::Color(45, 45, 45));
            }
        }
    }
}

void Modal::setSelectedTab(size_t index)
{
    if (index < m_tabs.size()) {
        m_selectedTab = index;
    }
}

void Modal::show()
{
    m_isVisible = true;
    m_ignoreNextEvent = true;
}

void Modal::addComponent(IComponent *component)
{
    m_components.push_back(component);
}

void Modal::updateTextSizes(TextSize size)
{
    for (auto &tabButton : m_tabButtons) {
        tabButton->updateTextSize(size);
    }
    if (m_saveButton) {
        m_saveButton->updateTextSize(size);
    }
    m_title.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::NORMAL_TEXT, size));
    m_text.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::NORMAL_TEXT, size));
}
