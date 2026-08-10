#pragma once

#include <SFML/Graphics.hpp>
#include <fstream>
#include <string>

using namespace std;

class InputManager {
public:
    InputManager();
    void loadSettings();
    void saveSettings();

    sf::Keyboard::Key getShootKey() const { return shootKey; }

    sf::Keyboard::Key getUpKey() const { return upKey; }

    sf::Keyboard::Key getDownKey() const { return downKey; }

    sf::Keyboard::Key getLeftKey() const { return leftKey; }

    sf::Keyboard::Key getRightKey() const { return rightKey; }

    sf::Keyboard::Key getBackKey() const { return backKey; }

    void setShootKey(sf::Keyboard::Key key) { shootKey = key; }

    void setUpKey(sf::Keyboard::Key key) { upKey = key; }

    void setDownKey(sf::Keyboard::Key key) { downKey = key; }

    void setLeftKey(sf::Keyboard::Key key) { leftKey = key; }

    void setRightKey(sf::Keyboard::Key key) { rightKey = key; }

    void setBackKey(sf::Keyboard::Key key) { backKey = key; }

    unsigned int getControllerShootButton() const
    {
        return controllerShootButton;
    }

    unsigned int getControllerBackButton() const
    {
        return controllerBackButton;
    }

    sf::Joystick::Axis getControllerMoveAxisX() const
    {
        return controllerMoveAxisX;
    }

    sf::Joystick::Axis getControllerMoveAxisY() const
    {
        return controllerMoveAxisY;
    }

    void setControllerShootButton(unsigned int button)
    {
        controllerShootButton = button;
    }

    void setControllerBackButton(unsigned int button)
    {
        controllerBackButton = button;
    }

    void setControllerMoveAxisX(sf::Joystick::Axis axis)
    {
        controllerMoveAxisX = axis;
    }

    void setControllerMoveAxisY(sf::Joystick::Axis axis)
    {
        controllerMoveAxisY = axis;
    }

    bool getInvertXAxis() const { return invertXAxis; }

    bool getInvertYAxis() const { return invertYAxis; }

    bool getUseLeftStick() const { return useLeftStick; }

    void setInvertXAxis(bool invert) { invertXAxis = invert; }

    void setInvertYAxis(bool invert) { invertYAxis = invert; }

    void setUseLeftStick(bool use) { useLeftStick = use; }

private:
    const string KEYBINDS_FILE = "keybinds.cfg";

    sf::Keyboard::Key shootKey = sf::Keyboard::Space;
    sf::Keyboard::Key upKey = sf::Keyboard::Z;
    sf::Keyboard::Key downKey = sf::Keyboard::S;
    sf::Keyboard::Key leftKey = sf::Keyboard::Q;
    sf::Keyboard::Key rightKey = sf::Keyboard::D;
    sf::Keyboard::Key backKey = sf::Keyboard::Escape;

    unsigned int controllerShootButton = 0;
    unsigned int controllerBackButton = 1;
    sf::Joystick::Axis controllerMoveAxisX = sf::Joystick::X;
    sf::Joystick::Axis controllerMoveAxisY = sf::Joystick::Y;
    bool invertXAxis = false;
    bool invertYAxis = false;
    bool useLeftStick = true;
};
