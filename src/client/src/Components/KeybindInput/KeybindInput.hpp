/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** KeybindInput
*/

#pragma once

#include "../TextStyle.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

using namespace std;

enum class InputType { Keyboard, ControllerButton, ControllerAxis };

class KeybindInput {
public:
    KeybindInput(sf::Vector2f position, sf::Vector2f size,
        const sf::Font &font, const string &label,
        InputType inputType = InputType::Keyboard);

    void draw(sf::RenderWindow &window) const;
    bool handleEvent(const sf::Event &event);
    void setKey(const sf::Keyboard::Key key);

    sf::Keyboard::Key getKey() const { return m_currentKey; }

    void show() { m_isVisible = true; }

    void hide() { m_isVisible = false; }

    bool isVisible() const { return m_isVisible; }

    void setJoystickButton(unsigned int button);
    void setJoystickAxis(sf::Joystick::Axis axis);

    void setJoystickButtonCallback(function<void(unsigned int)> callback);
    void setJoystickAxisCallback(function<void(sf::Joystick::Axis)> callback);

    void updateTextSize(const TextSize size);
    void recenterText();

    void setCallback(const function<void(sf::Keyboard::Key)> &callback)
    {
        m_onChange = callback;
    }

    static KeybindInput *activeInput;

private:
    sf::RectangleShape m_background;
    sf::Text m_label;
    sf::Text m_keyText;
    sf::Font m_font;
    bool m_isListening;
    sf::Keyboard::Key m_currentKey;
    function<void(sf::Keyboard::Key)> m_onChange;
    bool m_isVisible = true;

    InputType m_inputType;
    unsigned int m_currentJoystickButton;
    sf::Joystick::Axis m_currentJoystickAxis;
    function<void(unsigned int)> m_joystickButtonCallback;
    function<void(sf::Joystick::Axis)> m_joystickAxisCallback;

    TextSize m_currentTextSize;

    string getKeyName(sf::Keyboard::Key key);
    string getAxisName(sf::Joystick::Axis axis);
};
