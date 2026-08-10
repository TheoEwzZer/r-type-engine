/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** KeybindInput
*/

#include "KeybindInput.hpp"

KeybindInput *KeybindInput::activeInput = nullptr;

KeybindInput::KeybindInput(sf::Vector2f position, sf::Vector2f size,
    const sf::Font &font, const string &label, InputType inputType) :
    m_font(font),
    m_isListening(false), m_currentKey(sf::Keyboard::Unknown),
    m_inputType(inputType), m_currentTextSize(TextSize::MEDIUM)
{
    m_background.setPosition(position);
    m_background.setSize(size);
    m_background.setFillColor(sf::Color::White);
    m_background.setOutlineColor(sf::Color::Black);
    m_background.setOutlineThickness(2);

    m_label.setFont(font);
    m_label.setString(label);
    m_label.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::SETTINGS_LABEL, m_currentTextSize));
    m_label.setFillColor(sf::Color::Black);
    m_label.setPosition((position.x - 150), (position.y + (size.y / 4)));

    m_keyText.setFont(font);
    m_keyText.setString("Click to bind");
    m_keyText.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::SETTINGS_LABEL, m_currentTextSize));
    m_keyText.setFillColor(sf::Color::Black);
    m_keyText.setPosition((position.x + 10), (position.y + (size.y / 4)));
}

void KeybindInput::draw(sf::RenderWindow &window) const
{
    if (!m_isVisible) {
        return;
    }
    window.draw(m_background);
    window.draw(m_label);
    window.draw(m_keyText);
}

bool KeybindInput::handleEvent(const sf::Event &event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mousePos = { event.mouseButton.x, event.mouseButton.y };
        if (m_background.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            if (activeInput && activeInput != this) {
                activeInput->m_isListening = false;
                activeInput->m_background.setFillColor(sf::Color::White);
                activeInput->m_keyText.setString(
                    getKeyName(activeInput->m_currentKey));
            }
            activeInput = this;
            m_isListening = true;
            m_keyText.setString("Press any key...");
            m_background.setFillColor(sf::Color(200, 200, 200));
            return true;
        }
    } else if (m_isListening && activeInput == this) {
        if (m_inputType == InputType::Keyboard
            && event.type == sf::Event::KeyPressed) {
            sf::Keyboard::Key pressedKey = event.key.code;
            string keyName = getKeyName(pressedKey);
            if (keyName == "Impossible" || keyName == "Unknown") {
                m_keyText.setString("Key not allowed!");
                m_background.setFillColor(sf::Color(255, 200, 200));
                sf::Clock timer;
                while (timer.getElapsedTime().asSeconds() < 1.0f) {}
                m_background.setFillColor(sf::Color::White);
                m_keyText.setString(getKeyName(m_currentKey));
            } else {
                m_currentKey = pressedKey;
                m_keyText.setString(keyName);
                m_background.setFillColor(sf::Color::White);
                if (m_onChange) {
                    m_onChange(m_currentKey);
                }
            }
            m_isListening = false;
            activeInput = nullptr;
            return true;
        } else if (m_inputType == InputType::ControllerButton
            && event.type == sf::Event::JoystickButtonPressed) {
            unsigned int button = event.joystickButton.button;
            m_currentJoystickButton = button;
            m_keyText.setString("Button " + to_string(button));
            m_background.setFillColor(sf::Color::White);
            if (m_joystickButtonCallback) {
                m_joystickButtonCallback(button);
            }
            m_isListening = false;
            activeInput = nullptr;
            return true;
        } else if (m_inputType == InputType::ControllerAxis
            && event.type == sf::Event::JoystickMoved) {
            sf::Joystick::Axis axis = event.joystickMove.axis;
            m_currentJoystickAxis = axis;
            m_keyText.setString("Axis " + getAxisName(axis));
            m_background.setFillColor(sf::Color::White);
            if (m_joystickAxisCallback) {
                m_joystickAxisCallback(axis);
            }
            m_isListening = false;
            activeInput = nullptr;
            return true;
        }
    }

    return false;
}

void KeybindInput::setKey(const sf::Keyboard::Key key)
{
    m_currentKey = key;
    m_keyText.setString(getKeyName(key));
}

void KeybindInput::setJoystickButton(unsigned int button)
{
    m_currentJoystickButton = button;
    m_keyText.setString("Button " + to_string(button));
}

void KeybindInput::setJoystickAxis(sf::Joystick::Axis axis)
{
    m_currentJoystickAxis = axis;
    m_keyText.setString("Axis " + getAxisName(axis));
}

void KeybindInput::setJoystickButtonCallback(
    function<void(unsigned int)> callback)
{
    m_joystickButtonCallback = callback;
}

void KeybindInput::setJoystickAxisCallback(
    function<void(sf::Joystick::Axis)> callback)
{
    m_joystickAxisCallback = callback;
}

void KeybindInput::updateTextSize(const TextSize size)
{
    m_currentTextSize = size;
    m_label.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::SETTINGS_LABEL, m_currentTextSize));
    m_keyText.setCharacterSize(
        TextSizeConfig::getSize(TextStyle::SETTINGS_LABEL, m_currentTextSize));
    recenterText();
}

void KeybindInput::recenterText()
{
    sf::FloatRect labelBounds = m_label.getLocalBounds();
    sf::FloatRect keyTextBounds = m_keyText.getLocalBounds();

    m_label.setPosition(m_background.getPosition().x - labelBounds.width - 10,
        m_background.getPosition().y
            + (m_background.getSize().y - labelBounds.height) / 2.0f);

    m_keyText.setPosition(m_background.getPosition().x + 10,
        m_background.getPosition().y
            + (m_background.getSize().y - keyTextBounds.height) / 2.0f);
}

string KeybindInput::getKeyName(sf::Keyboard::Key key)
{
    switch (key) {
        case sf::Keyboard::Space:
            return "Space";
        case sf::Keyboard::A:
            return "A";
        case sf::Keyboard::B:
            return "B";
        case sf::Keyboard::C:
            return "C";
        case sf::Keyboard::D:
            return "D";
        case sf::Keyboard::E:
            return "E";
        case sf::Keyboard::F:
            return "F";
        case sf::Keyboard::G:
            return "G";
        case sf::Keyboard::H:
            return "H";
        case sf::Keyboard::I:
            return "I";
        case sf::Keyboard::J:
            return "J";
        case sf::Keyboard::K:
            return "K";
        case sf::Keyboard::L:
            return "L";
        case sf::Keyboard::M:
            return "M";
        case sf::Keyboard::N:
            return "N";
        case sf::Keyboard::O:
            return "O";
        case sf::Keyboard::P:
            return "P";
        case sf::Keyboard::Q:
            return "Q";
        case sf::Keyboard::R:
            return "R";
        case sf::Keyboard::S:
            return "S";
        case sf::Keyboard::T:
            return "T";
        case sf::Keyboard::U:
            return "U";
        case sf::Keyboard::V:
            return "V";
        case sf::Keyboard::W:
            return "W";
        case sf::Keyboard::X:
            return "X";
        case sf::Keyboard::Y:
            return "Y";
        case sf::Keyboard::Z:
            return "Z";
        case sf::Keyboard::Num0:
            return "0";
        case sf::Keyboard::Num1:
            return "1";
        case sf::Keyboard::Num2:
            return "2";
        case sf::Keyboard::Num3:
            return "3";
        case sf::Keyboard::Num4:
            return "4";
        case sf::Keyboard::Num5:
            return "5";
        case sf::Keyboard::Num6:
            return "6";
        case sf::Keyboard::Num7:
            return "7";
        case sf::Keyboard::Num8:
            return "8";
        case sf::Keyboard::Num9:
            return "9";

        case sf::Keyboard::Escape:
            return "Impossible";
        case sf::Keyboard::LControl:
            return "LControl";
        case sf::Keyboard::LShift:
            return "LShift";
        case sf::Keyboard::LAlt:
            return "LAlt";
        case sf::Keyboard::RControl:
            return "RControl";
        case sf::Keyboard::RShift:
            return "RShift";
        case sf::Keyboard::LBracket:
            return "[";
        case sf::Keyboard::RBracket:
            return "]";
        case sf::Keyboard::Return:
            return "Return";

        case sf::Keyboard::Unknown:
            return "Unknown";
        default:
            return "Unknown";
    }
}

string KeybindInput::getAxisName(sf::Joystick::Axis axis)
{
    switch (axis) {
        case sf::Joystick::X:
            return "X";
        case sf::Joystick::Y:
            return "Y";
        case sf::Joystick::Z:
            return "Z";
        case sf::Joystick::R:
            return "R";
        case sf::Joystick::U:
            return "U";
        case sf::Joystick::V:
            return "V";
        case sf::Joystick::PovX:
            return "PovX";
        case sf::Joystick::PovY:
            return "PovY";
        default:
            return "Unknown";
    }
}
