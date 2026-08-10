/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** ClientEngine
*/

#pragma once

#include "../IEngine.hpp"
#include "../Utils/InputManager/InputManager.hpp"
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

class ClientEngine : public IEngine {
public:
    ClientEngine() : _ip("127.0.0.1") { initializeAudio(); }

    explicit ClientEngine(const char *ip) : _ip(ip) { initializeAudio(); }

    ~ClientEngine() = default;

    void start();
    void applySetting(const string &key, const string &value);

    sf::RenderWindow &getWindow() { return window; }

    const string &getIp() const { return _ip; }

    float getMasterVolume() const { return masterVolume; }

    float getMusicVolume() const { return musicVolume; }

    float getEffectsVolume() const { return effectsVolume; }

    void setMasterVolume(float volume);

    void setMusicVolume(float volume);

    void setEffectsVolume(float volume);

    void initializeAudio();

    InputManager &getInputManager() { return inputManager; }

    void loadSettings();
    void saveSettings();
    void loadSound(const string &name, const string &filename);
    void playSound(const string &name);
    void loadMusic(const string &id, const string &filename);
    void playMusic(const string &id);
    void stopMusic(const string &id);
    void setMusicLoop(const string &id, bool loop);
    void updateVolumes();
    void saveScore();
    void loadScore();
    void updateScore(unsigned int newScore);

    unsigned int getBestScore() const { return bestScore; }

    unsigned int getCurrentScore() const { return currentScore; }

private:
    sf::RenderWindow window;
    map<string, unique_ptr<sf::SoundBuffer>> _soundBuffers;
    map<string, unique_ptr<sf::Sound>> _sounds;
    map<string, unique_ptr<sf::Music>> _musics;
    unique_ptr<sf::Music> _currentMusic;
    float masterVolume = 0;
    float musicVolume = 0;
    float effectsVolume = 0;
    const string SETTINGS_FILE = "settings.cfg";
    string _ip;
    InputManager inputManager;
    unsigned int currentScore = 0;
    unsigned int bestScore = 0;
    static constexpr const char *SCORE_FILE = "scores.cfg";
};
