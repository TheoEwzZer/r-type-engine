/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Select
*/

#include "Select.hpp"

Select::Select(const sf::Vector2f position, const sf::Vector2f size,
    const sf::Font &font, const string &title) :
    m_isOpen(false),
    m_font(font), m_currentTextSize(TextSize::MEDIUM), m_title()
{
    m_mainBox.setPosition(position);
    m_mainBox.setSize(size);
    m_mainBox.setFillColor(sf::Color::White);
    m_mainBox.setOutlineColor(sf::Color::Black);
    m_mainBox.setOutlineThickness(2);

    m_text.setFont(font);
    m_text.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::SETTINGS_LABEL, m_currentTextSize));
    m_text.setFillColor(sf::Color::Black);
    m_text.setPosition(position.x + 10, position.y + size.y / 4);

    m_title.setFont(font);
    m_title.setString(title);
    m_title.setCharacterSize(20);
    m_title.setFillColor(sf::Color::Black);
    m_title.setPosition(position.x - m_title.getLocalBounds().width - 20,
        position.y + (size.y - m_title.getLocalBounds().height) / 2.0f);
}

void Select::addOption(const string &option)
{
    m_options.push_back(option);
    if (m_options.size() == 1) {
        m_selectedOption = option;
        m_text.setString(option);
    }
    updateDisplay();
}

void Select::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible) {
        return;
    }

    window.draw(m_mainBox);
    window.draw(m_text);
    window.draw(m_title);

    if (m_isOpen) {
        const size_t boxCount = m_optionBoxes.size();
        for (size_t i = 0; i < boxCount; ++i) {
            window.draw(m_optionBoxes[i]);
            window.draw(m_optionTexts[i]);
        }
    }
}

void Select::handleEvent(
    const sf::Event &event, const sf::RenderWindow &window)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        const sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        if (m_mainBox.getGlobalBounds().contains(
                static_cast<float>(mousePos.x),
                static_cast<float>(mousePos.y))) {
            m_isOpen = !m_isOpen;
            return;
        }

        if (m_isOpen) {
            const size_t boxCount = m_optionBoxes.size();
            for (size_t i = 0; i < boxCount; ++i) {
                if (m_optionBoxes[i].getGlobalBounds().contains(
                        static_cast<float>(mousePos.x),
                        static_cast<float>(mousePos.y))) {
                    setSelectedOption(m_options[i]);
                    m_isOpen = false;
                    return;
                }
            }
        }

        m_isOpen = false;
    }
}

void Select::setCallback(const function<void(const string &)> &callback)
{
    m_callback = callback;
}

void Select::setSelectedOption(const string &option)
{
    if (ranges::find(m_options, option) != m_options.end()) {
        m_selectedOption = option;
        m_text.setString(option);
        if (m_callback) {
            m_callback(option);
        }
    }
}

string Select::getSelectedOption() const { return m_selectedOption; }

void Select::updateDisplay()
{
    m_optionBoxes.clear();
    m_optionTexts.clear();

    const float boxHeight = m_mainBox.getSize().y;

    const size_t optionCount = m_options.size();
    for (size_t i = 0; i < optionCount; ++i) {
        sf::RectangleShape optionBox;
        optionBox.setSize(m_mainBox.getSize());
        optionBox.setPosition(m_mainBox.getPosition().x,
            m_mainBox.getPosition().y
                + (static_cast<float>(i) + 1.0f) * boxHeight);
        optionBox.setFillColor(sf::Color::White);
        optionBox.setOutlineColor(sf::Color::Black);
        optionBox.setOutlineThickness(1);

        sf::Text optionText;
        optionText.setFont(m_font);
        optionText.setString(m_options[i]);
        optionText.setCharacterSize(TextSizeConfig::getSize(
            TextStyle::SETTINGS_LABEL, m_currentTextSize));
        optionText.setFillColor(sf::Color::Black);

        optionText.setPosition(optionBox.getPosition().x + 10,
            optionBox.getPosition().y
                + (boxHeight - optionText.getLocalBounds().height) / 2.f);

        m_optionBoxes.push_back(optionBox);
        m_optionTexts.push_back(optionText);
    }
}

void Select::updateTextSize(const TextSize size)
{
    m_currentTextSize = size;
    m_text.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::SETTINGS_LABEL, m_currentTextSize));

    for (auto &text : m_optionTexts) {
        text.setCharacterSize(TextSizeConfig::getSize(
            TextStyle::SETTINGS_LABEL, m_currentTextSize));
    }

    recenterText();
}

void Select::recenterText()
{
    const sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setPosition(m_mainBox.getPosition().x + 10,
        m_mainBox.getPosition().y
            + (m_mainBox.getSize().y - textBounds.height) / 2.0f);

    const float boxHeight = m_mainBox.getSize().y;
    const size_t optionCount = m_optionTexts.size();
    for (size_t i = 0; i < optionCount; ++i) {
        const sf::FloatRect optionBounds = m_optionTexts[i].getLocalBounds();
        m_optionTexts[i].setPosition(m_mainBox.getPosition().x + 10,
            m_mainBox.getPosition().y
                + (static_cast<float>(i) + 1.0f) * boxHeight
                + (boxHeight - optionBounds.height) / 2.0f);
    }
}

void Select::setTitle(const string &title) { m_title.setString(title); }
