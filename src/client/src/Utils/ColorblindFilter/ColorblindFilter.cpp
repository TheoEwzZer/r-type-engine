/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Colorblind
*/

#include "ColorblindFilter.hpp"

ColorblindFilter::ColorblindFilter()
{
    if (!m_colorblindShader.loadFromFile(
            "src/client/assets/shaders/colorblind_filter.frag",
            sf::Shader::Fragment)) {
        throw ColorblindFilterException("Failed to load colorblind shader");
    }

    setMode(Mode::PROTANOPIA);
}

ColorblindFilter::~ColorblindFilter() {}

void ColorblindFilter::setMode(Mode mode)
{
    m_currentMode = mode;

    switch (mode) {
        case Mode::PROTANOPIA:
            m_colorblindShader.setUniform("u_mode", 0);
            break;
        case Mode::DEUTERANOPIA:
            m_colorblindShader.setUniform("u_mode", 1);
            break;
        case Mode::TRITANOPIA:
            m_colorblindShader.setUniform("u_mode", 2);
            break;
    }
}

ColorblindFilter::Mode ColorblindFilter::getCurrentMode() const
{
    return m_currentMode;
}

void ColorblindFilter::prepareTexture(
    sf::RenderWindow &window, const sf::Drawable &scene)
{
    if (!m_renderTexture.create(window.getSize().x, window.getSize().y)) {
        throw ColorblindFilterException("Failed to create render texture");
    }

    m_renderTexture.clear();
    m_renderTexture.draw(scene);
    m_renderTexture.display();
    m_sceneSprite.setTexture(m_renderTexture.getTexture(), true);
}

void ColorblindFilter::draw(sf::RenderWindow &window)
{
    window.draw(m_sceneSprite, &m_colorblindShader);
}
