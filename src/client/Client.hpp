/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Client
*/

#pragma once

#include "ClientEngine/ClientEngine.hpp"
#include "Protocol.hpp"
#include "src/Utils/InputManager/InputManager.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace asio;
using namespace asio::ip;
using namespace std::chrono;
using namespace rtype;
using enum rtype::Direction;

namespace rtype {

static const unsigned short spritesNumber = 12;

class Particle {
public:
    Particle() = default;

    sf::RectangleShape shape;
    sf::Vector2f velocity;
    int lifespan;
    float initialSize;
};

class Client {
public:
    Client(sf::RenderWindow &window, InputManager &inputManager,
        bool spectatorMode = false, int skin = 0);
    void run();

    void updateScore(unsigned int newScore)
    {
        clientEngine->updateScore(newScore);
        scoreText.setString("Score " + to_string(newScore));
    }

    void setGameOver() { isGameOver = true; }

private:
    struct SpriteState {
        sf::Clock collisionTimer;
        bool isInCollision = false;
        sf::Color originalColor;
    };

    map<string, unique_ptr<sf::Music>, less<>> musics;

    unordered_map<unsigned int, unordered_set<unsigned int>> collisionsMap;

    unordered_map<unsigned int, SpriteState> spriteStates;
    int getSpriteSheetIndex(const sf::Sprite &sprite) const;

    bool checkCollision(
        const sf::Sprite &sprite1, const sf::Sprite &sprite2) const;
    void handleCollisions();
    void updateSpriteColors();
    void handleSocketOpen();
    void handleClientIdReception();
    void handlePlayerInput(Direction &dx, Direction &dy);
    void handleSocketSend(const Direction dx, const Direction dy);
    void handleSocketReceive();
    void renderWindow();
    void updateParticles();
    void renderParticles();
    sf::Sprite createSprite(const Sprite &sprite);
    void fireProjectile(const Event shootEvent);
    void fadeIn();
    void fadeOut();

    vector<Particle> particles;
    asio::io_context ioContext;
    asio::ip::udp::socket socket;
    asio::ip::udp::endpoint serverEndpoint;
    sf::RenderWindow &window;
    sf::Clock gameClock;
    map<unsigned int, sf::Sprite> sprites;
    unordered_map<unsigned int, sf::Texture> textures;
    bool lastSentNone = false;
    unsigned int clientId;
    bool clientIdReceived;
    vector<unsigned char> connectBuffer;
    steady_clock::time_point lastSend;
    InputManager &inputManager;
    steady_clock::time_point lastProjectileTime;
    bool isCharging = false;
    steady_clock::time_point chargeStartTime;
    int playerLives = 3;
    int level = 1;
    sf::Font font;
    sf::Text livesText;
    sf::Sprite backgroundSprite1;
    sf::Sprite backgroundSprite2;
    float currentBackgroundWidth = 376.0f;
    sf::RectangleShape fadeRect;
    sf::Uint8 fadeAlpha = 255;
    unique_ptr<ClientEngine> clientEngine;
    bool isBossFight = false;
    sf::Sprite chargeIndicator;
    sf::Clock chargeAnimationClock;
    int chargeAnimationFrame = 0;
    static const int CHARGE_ANIMATION_FRAMES = 9;
    static const int CHARGE_ANIMATION_FRAME_WIDTH = 32;
    bool normalShotDisplayed = false;
    sf::Clock normalShotTimer;
    bool wasEKeyPressed = false;
    sf::Text scoreText;
    bool isGameOver = false;
    void showGameOverScreen();
    bool showSoundSprite = false;
    sf::Texture soundTexture;
    sf::Sprite soundSprite;
    bool spectatorMode;
    int skin;
    sf::Text lagText;
    int64_t duration = 0;
};
}
