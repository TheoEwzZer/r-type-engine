/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** ClientEngine
*/

#include "ClientEngine.hpp"
#include <SFML/Audio.hpp>

void ClientEngine::loadScore()
{
    ifstream file(SCORE_FILE);
    if (!file.is_open()) {
        bestScore = 0;
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

        if (key == "bestScore") {
            bestScore = stoul(value);
        }
    }
}

void ClientEngine::saveScore()
{
    ofstream file(SCORE_FILE);
    if (!file.is_open()) {
        cerr << "Could not save score" << endl;
        return;
    }
    file << "bestScore=" << bestScore << "\n";
}

void ClientEngine::updateScore(unsigned int newScore)
{
    currentScore = newScore;
    if (currentScore > bestScore) {
        bestScore = currentScore;
        saveScore();
    }
}

void ClientEngine::playSound(const string &name)
{
    auto it = _sounds.find(name);
    if (it != _sounds.end()) {
        it->second->play();
    } else {
        cerr << "Sound not found: " << name << endl;
    }
}

void ClientEngine::loadSound(const string &name, const string &filename)
{
    auto buffer = make_unique<sf::SoundBuffer>();
    if (!buffer->loadFromFile(filename)) {
        return;
    }
    _soundBuffers[name] = move(buffer);
    _sounds[name] = make_unique<sf::Sound>(*_soundBuffers[name]);
    updateVolumes();
}

void ClientEngine::loadMusic(const string &id, const string &filename)
{
    auto music = make_unique<sf::Music>();
    if (!music->openFromFile(filename)) {
        cerr << "Failed to load music: " << filename << endl;
        return;
    }
    music->setLoop(true);
    _musics[id] = move(music);
    updateVolumes();
}

void ClientEngine::playMusic(const string &id)
{
    auto it = _musics.find(id);
    if (it != _musics.end()) {
        it->second->play();
    }
}

void ClientEngine::stopMusic(const string &id)
{
    auto it = _musics.find(id);
    if (it != _musics.end()) {
        it->second->stop();
    }
}

void ClientEngine::setMusicLoop(const string &id, bool loop)
{
    auto it = _musics.find(id);
    if (it != _musics.end()) {
        it->second->setLoop(loop);
    }
}

void ClientEngine::setMasterVolume(float volume)
{
    masterVolume = volume;
    updateVolumes();
}

void ClientEngine::setMusicVolume(float volume)
{
    musicVolume = volume;
    updateVolumes();
}

void ClientEngine::setEffectsVolume(float volume)
{
    effectsVolume = volume;
    updateVolumes();
}

void ClientEngine::updateVolumes()
{
    float musicVol = (masterVolume / 100.0f) * (musicVolume / 100.0f) * 100.0f;
    float effectsVol
        = (masterVolume / 100.0f) * (effectsVolume / 100.0f) * 100.0f;

    for (auto &mPair : _musics) {
        mPair.second->setVolume(musicVol);
    }

    for (auto &sPair : _sounds) {
        sPair.second->setVolume(effectsVol);
    }
}

void ClientEngine::initializeAudio()
{
    loadSettings();
    loadScore();
    loadSound("shoot", "src/client/assets/sounds/shoot.wav");
    loadSound("explosion", "src/client/assets/sounds/explosion.wav");
    loadSound("powerup", "src/client/assets/sounds/powerup.wav");
    loadSound("kill", "src/client/assets/sounds/kill.wav");
    loadSound("death", "src/client/assets/sounds/death.wav");

    playMusic("src/client/assets/music/background.ogg");

    loadMusic("level1", "src/client/assets/sounds/start.wav");
    loadMusic("boss", "src/client/assets/sounds/boss.wav");
    loadMusic("level2", "src/client/assets/sounds/2nd.wav");
}

void ClientEngine::loadSettings()
{
    ifstream file(SETTINGS_FILE);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            size_t pos = line.find('=');
            if (pos == string::npos) {
                continue;
            }

            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);

            if (key.empty() || value.empty()) {
                continue;
            }

            applySetting(key, value);
        }
        file.close();
    }

    inputManager.loadSettings();
}

void ClientEngine::applySetting(const string &key, const string &value)
{
    auto &inputs = inputManager;
    if (key == "masterVolume") {
        setMasterVolume(stof(value));
    } else if (key == "musicVolume") {
        setMusicVolume(stof(value));
    } else if (key == "effectsVolume") {
        setEffectsVolume(stof(value));
    } else if (key == "shootKey") {
        inputs.setShootKey(static_cast<sf::Keyboard::Key>(stoi(value)));
    } else if (key == "upKey") {
        inputs.setUpKey(static_cast<sf::Keyboard::Key>(stoi(value)));
    } else if (key == "downKey") {
        inputs.setDownKey(static_cast<sf::Keyboard::Key>(stoi(value)));
    } else if (key == "leftKey") {
        inputs.setLeftKey(static_cast<sf::Keyboard::Key>(stoi(value)));
    } else if (key == "rightKey") {
        inputs.setRightKey(static_cast<sf::Keyboard::Key>(stoi(value)));
    } else if (key == "controllerShootButton") {
        inputs.setControllerShootButton(stoi(value));
    } else if (key == "invertXAxis") {
        inputs.setInvertXAxis(value == "1");
    } else if (key == "invertYAxis") {
        inputs.setInvertYAxis(value == "1");
    } else if (key == "controllerBackButton") {
        inputs.setControllerBackButton(stoi(value));
    }
}

void ClientEngine::saveSettings()
{
    ofstream file(SETTINGS_FILE);
    if (file.is_open()) {
        file << "masterVolume=" << getMasterVolume() << "\n";
        file << "musicVolume=" << getMusicVolume() << "\n";
        file << "effectsVolume=" << getEffectsVolume() << "\n";

        const auto &inputs = inputManager;
        file << "shootKey=" << static_cast<int>(inputs.getShootKey()) << "\n";
        file << "upKey=" << static_cast<int>(inputs.getUpKey()) << "\n";
        file << "downKey=" << static_cast<int>(inputs.getDownKey()) << "\n";
        file << "leftKey=" << static_cast<int>(inputs.getLeftKey()) << "\n";
        file << "rightKey=" << static_cast<int>(inputs.getRightKey()) << "\n";
        file << "controllerShootButton=" << inputs.getControllerShootButton()
             << "\n";
        file << "invertXAxis=" << (inputs.getInvertXAxis() ? "1" : "0")
             << "\n";
        file << "invertYAxis=" << (inputs.getInvertYAxis() ? "1" : "0")
             << "\n";
        file << "controllerBackButton=" << inputs.getControllerBackButton()
             << "\n";

        file.close();
    }
    inputManager.saveSettings();
}
