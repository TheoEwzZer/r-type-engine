#include "InputManager.hpp"
#include <iostream>

InputManager::InputManager() { loadSettings(); }

void InputManager::loadSettings()
{
    ifstream file(KEYBINDS_FILE);
    if (!file.is_open()) {
        saveSettings();
        return;
    }

    string line;
    while (getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == string::npos) {
            continue;
        }

        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);

        if (key == "shootKey") {
            setShootKey(static_cast<sf::Keyboard::Key>(stoi(value)));
        } else if (key == "upKey") {
            setUpKey(static_cast<sf::Keyboard::Key>(stoi(value)));
        } else if (key == "downKey") {
            setDownKey(static_cast<sf::Keyboard::Key>(stoi(value)));
        } else if (key == "leftKey") {
            setLeftKey(static_cast<sf::Keyboard::Key>(stoi(value)));
        } else if (key == "rightKey") {
            setRightKey(static_cast<sf::Keyboard::Key>(stoi(value)));
        } else if (key == "backKey") {
            setBackKey(static_cast<sf::Keyboard::Key>(stoi(value)));
        } else if (key == "controllerShootButton") {
            setControllerShootButton(stoi(value));
        } else if (key == "controllerBackButton") {
            setControllerBackButton(stoi(value));
        } else if (key == "invertXAxis") {
            setInvertXAxis(value == "1");
        } else if (key == "invertYAxis") {
            setInvertYAxis(value == "1");
        } else if (key == "useLeftStick") {
            setUseLeftStick(value == "1");
        }
    }
}

void InputManager::saveSettings()
{
    ofstream file(KEYBINDS_FILE);
    if (!file.is_open()) {
        cerr << "Could not save keybinds\n";
        return;
    }

    file << "shootKey=" << static_cast<int>(shootKey) << "\n"
         << "upKey=" << static_cast<int>(upKey) << "\n"
         << "downKey=" << static_cast<int>(downKey) << "\n"
         << "leftKey=" << static_cast<int>(leftKey) << "\n"
         << "rightKey=" << static_cast<int>(rightKey) << "\n"
         << "backKey=" << static_cast<int>(backKey) << "\n"
         << "controllerShootButton=" << controllerShootButton << "\n"
         << "controllerBackButton=" << controllerBackButton << "\n"
         << "invertXAxis=" << (invertXAxis ? "1" : "0") << "\n"
         << "invertYAxis=" << (invertYAxis ? "1" : "0") << "\n"
         << "useLeftStick=" << (useLeftStick ? "1" : "0") << "\n";
}
