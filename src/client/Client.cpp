/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Client
*/

#include "Client.hpp"
#include "src/Network/AsioNetworkClient.hpp"

Client::Client(sf::RenderWindow &window, InputManager &inputManager,
    bool spectatorMode, int skin) :
    network(std::make_unique<AsioNetworkClient>("127.0.0.1", 4242)), window(window),
    clientId(0), clientIdReceived(false), inputManager(inputManager),
    lastProjectileTime(steady_clock::now()),
    clientEngine(make_unique<ClientEngine>()), spectatorMode(spectatorMode),
    skin(skin)
{
    connectBuffer = BinaryProtocol::serializePlayerEvent(
        { Event::JOIN, static_cast<unsigned int>(skin) });
    for (unsigned int i = 1; i <= spritesNumber; ++i) {
        const string texturePath
            = format("src/client/assets/r-typesheet{}.png", i);
        sf::Texture texture;
        texture.loadFromFile(texturePath);
        textures[i] = texture;
    }
    if (!soundTexture.loadFromFile("src/client/assets/sound.png")) {
        throw runtime_error("Failed to load sound.png");
    }
    soundSprite.setTexture(soundTexture);
    soundSprite.setScale(0.25f, 0.25f);
    soundSprite.setPosition(
        window.getSize().x - soundSprite.getGlobalBounds().width - 20,
        window.getSize().y - soundSprite.getGlobalBounds().height - 20);
    backgroundSprite1.setTexture(textures[4]);
    backgroundSprite2.setTexture(textures[4]);
    backgroundSprite1.setPosition(0.0f, 0.0f);
    backgroundSprite2.setPosition(1128.0f, 0.0f);
    backgroundSprite1.setTextureRect(
        sf::IntRect(0, 0, static_cast<int>(currentBackgroundWidth), 224));
    backgroundSprite2.setTextureRect(
        sf::IntRect(0, 0, static_cast<int>(currentBackgroundWidth), 224));
    backgroundSprite1.setScale(1128 / currentBackgroundWidth, 672 / 224.0f);
    backgroundSprite2.setScale(1128 / currentBackgroundWidth, 672 / 224.0f);
    chargeIndicator.setTexture(textures[1]);
    chargeIndicator.setScale(1.5f, 1.5f);
    chargeIndicator.setTextureRect(
        sf::IntRect(0, 51, CHARGE_ANIMATION_FRAME_WIDTH, 32));
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(window.getSize().x - 250, 10);
    scoreText.setString("Score 0");
    lagText.setFont(font);
    lagText.setCharacterSize(24);
    lagText.setFillColor(sf::Color::White);
    lagText.setPosition(10, 40);
    lagText.setString("Lag: 0 ms");
    
    comboText.setFont(font);
    comboText.setCharacterSize(28);
    comboText.setFillColor(sf::Color(255, 200, 0, 255));
    comboText.setStyle(sf::Text::Bold);
    comboText.setPosition(window.getSize().x - 250, 45);
    comboText.setString("");

    mainView = window.getDefaultView();
}

void Client::triggerScreenShake(float durationSec, float intensity)
{
    screenShakeDuration = durationSec;
    screenShakeIntensity = intensity;
}

void Client::showGameOverScreen()
{
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(
        (window.getSize().x - gameOverText.getGlobalBounds().width) / 2,
        window.getSize().y / 3);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Final Score "
        + to_string(clientEngine->getCurrentScore()) + "\n\nBest Score "
        + to_string(clientEngine->getBestScore()));
    scoreText.setPosition(
        (window.getSize().x - scoreText.getGlobalBounds().width) / 2,
        window.getSize().y / 2);

    sf::RectangleShape exitButton(sf::Vector2f(200, 50));
    exitButton.setFillColor(sf::Color(100, 100, 100));
    exitButton.setPosition((window.getSize().x - exitButton.getSize().x) / 2,
        window.getSize().y * 0.7f);

    sf::Text exitText;
    exitText.setFont(font);
    exitText.setString("Exit");
    exitText.setCharacterSize(24);
    exitText.setFillColor(sf::Color::White);
    exitText.setPosition(exitButton.getPosition().x
            + (exitButton.getSize().x - exitText.getGlobalBounds().width) / 2,
        exitButton.getPosition().y
            + (exitButton.getSize().y - exitText.getGlobalBounds().height)
                / 2);

    while (window.isOpen() && isGameOver) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (exitButton.getGlobalBounds().contains(
                        mousePos.x, mousePos.y)) {
                    window.close();
                }
            }
        }

        window.clear(sf::Color(0, 0, 0, 200));
        window.draw(gameOverText);
        window.draw(scoreText);
        window.draw(exitButton);
        window.draw(exitText);
        window.display();
    }
}

void Client::handleSocketOpen()
{
    try {
        network->open();
    } catch (std::exception &e) {
        ::exit(0);
    }
}

sf::Sprite Client::createSprite(const Sprite &sprite)
{
    sf::Sprite gameSprite;
    gameSprite.setTexture(textures[sprite.spritesheetIndex]);
    gameSprite.setTextureRect(
        sf::IntRect(sprite.x, sprite.y, sprite.width, sprite.height));
    gameSprite.setPosition(
        static_cast<float>(sprite.gameX), static_cast<float>(sprite.gameY));
    gameSprite.setScale(sprite.scaleX, sprite.scaleY);
    if (sprite.spritesheetIndex == 8) {
        gameSprite.setOrigin(static_cast<float>(sprite.width / 2),
            static_cast<float>(sprite.height / 2));
    }
    return gameSprite;
}

void Client::updateParticles()
{
    const float lifespanFactor = 1.0f / 100.0f;
    const float maxAlpha = 255.0f;

    for (auto it = particles.begin(); it != particles.end();) { // NOSONAR
        const float lifespanRatio
            = static_cast<float>(it->lifespan) * lifespanFactor;

        it->shape.move(it->velocity);
        --it->lifespan;

        sf::Color color = it->shape.getFillColor();
        color.a = static_cast<sf::Uint8>(maxAlpha * lifespanRatio);
        it->shape.setFillColor(color);

        const float newSize = it->initialSize * lifespanRatio;
        it->shape.setSize(sf::Vector2f(newSize, newSize));

        it = (it->lifespan <= 0) ? particles.erase(it) : next(it);
    }
}

void Client::spawnExplosionParticles(sf::Vector2f position, int count, sf::Color color)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> distVelocity(-3.0f, 3.0f);
    uniform_int_distribution<int> distLifespan(20, 60);
    uniform_real_distribution<float> distSize(2.0f, 8.0f);

    for (int i = 0; i < count; ++i) {
        Particle particle;
        float size = distSize(gen);
        particle.shape.setSize(sf::Vector2f(size, size));
        particle.shape.setFillColor(color);
        particle.shape.setPosition(position);
        particle.velocity = sf::Vector2f(distVelocity(gen), distVelocity(gen));
        particle.lifespan = distLifespan(gen);
        particle.initialSize = size;
        particles.emplace_back(particle);
    }
}

void Client::renderParticles()
{
    for (const auto &particle : particles) {
        window.draw(particle.shape);
    }
}

void Client::handleClientIdReception()
{
    static auto lastJoinSendTime = steady_clock::now();
    const auto now = steady_clock::now();
    if (duration_cast<milliseconds>(now - lastJoinSendTime).count() > 1000) {
        network->send(connectBuffer);
        lastJoinSendTime = now;
    }

    vector<unsigned char> recvBuffer(65'504);
    size_t length = 0;
    network->setNonBlocking(true);
    length = network->receive(recvBuffer);
    if (!network->isWouldBlock() && length > 0) {
        recvBuffer.resize(length);
        if (recvBuffer[0] == static_cast<unsigned char>(Event::JOIN)) {
            const PlayerEvent event
                = BinaryProtocol::deserializePlayerEvent(recvBuffer);
            if (event.event == Event::JOIN) {
                clientId = event.playerId;
                clientIdReceived = true;
                cout << "Received client ID: " << clientId << "\n";
            }
        }
    }
}

void Client::handlePlayerInput(Direction &dx, Direction &dy)
{
    if (sf::Keyboard::isKeyPressed(inputManager.getLeftKey())) {
        dx = LEFT;
    }
    if (sf::Keyboard::isKeyPressed(inputManager.getRightKey())) {
        dx = RIGHT;
    }
    if (sf::Keyboard::isKeyPressed(inputManager.getUpKey())) {
        dy = UP;
    }
    if (sf::Keyboard::isKeyPressed(inputManager.getDownKey())) {
        dy = DOWN;
    }

    if (sf::Joystick::isConnected(0)) {
        float axisX = sf::Joystick::getAxisPosition(
            0, inputManager.getControllerMoveAxisX());
        float axisY = sf::Joystick::getAxisPosition(
            0, inputManager.getControllerMoveAxisY());

        if (inputManager.getInvertXAxis()) {
            axisX = -axisX;
        }
        if (inputManager.getInvertYAxis()) {
            axisY = -axisY;
        }

        const float deadzone = 20.0f;
        if (abs(axisX) > deadzone) {
            dx = (axisX < 0) ? LEFT : RIGHT;
        }
        if (abs(axisY) > deadzone) {
            dy = (axisY < 0) ? UP : DOWN;
        }

        if (sf::Joystick::isButtonPressed(
                0, inputManager.getControllerShootButton())
            && (!isCharging)) {
            isCharging = true;
            chargeStartTime = steady_clock::now();
        }
        if ((!sf::Joystick::isButtonPressed(
                0, inputManager.getControllerShootButton()))
            && isCharging) {
            isCharging = false;
            const auto chargeDuration = duration_cast<milliseconds>(
                steady_clock::now() - chargeStartTime)
                                            .count();
            if (chargeDuration >= 500) {
                triggerScreenShake(0.1f, 1.5f);
                fireProjectile(Event::CHARGED_SHOOT);
            } else {
                fireProjectile(Event::SHOOT);
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(inputManager.getShootKey())
        && (!isCharging)) {
        isCharging = true;
        chargeStartTime = steady_clock::now();
    }
    if ((!sf::Keyboard::isKeyPressed(inputManager.getShootKey()))
        && isCharging) {
        isCharging = false;
        const auto chargeDuration = duration_cast<milliseconds>(
            steady_clock::now() - chargeStartTime)
                                        .count();
        if (chargeDuration >= 500) {
            triggerScreenShake(0.1f, 1.5f); // Reduced intensity
            fireProjectile(Event::CHARGED_SHOOT);
        } else {
            fireProjectile(Event::SHOOT);
        }
    }

    if (sf::Keyboard::isKeyPressed(inputManager.getBackKey())
        || (sf::Joystick::isConnected(0)
            && sf::Joystick::isButtonPressed(
                0, inputManager.getControllerBackButton()))) {
        window.close();
        PlayerEvent event { Event::DESTROY, clientId, nextPacketId++ };
        pendingEvents.push_back({event, steady_clock::now()});
        auto buffer = BinaryProtocol::serializePlayerEvent(event);
        network->send(buffer);
    }

    bool isEKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
    if (isEKeyPressed && !wasEKeyPressed) {
        PlayerEvent event { Event::DETACH_ATTACH_FORCE, clientId, nextPacketId++ };
        pendingEvents.push_back({event, steady_clock::now()});
        auto buffer = BinaryProtocol::serializePlayerEvent(event);
        network->send(buffer);
    }
    wasEKeyPressed = isEKeyPressed;
}

void Client::fireProjectile(const Event shootEvent)
{
    const auto now = steady_clock::now();
    const auto duration
        = duration_cast<milliseconds>(now - lastProjectileTime).count();
    if (duration < 300) {
        return;
    }
    const PlayerEvent event { shootEvent, clientId, nextPacketId++ };
    pendingEvents.push_back({event, steady_clock::now()});
    auto buffer = BinaryProtocol::serializePlayerEvent(event);
    network->send(buffer);
    lastProjectileTime = now;
    clientEngine->playSound("shoot");
    chargeAnimationFrame = 0;
    chargeIndicator.setTextureRect(
        sf::IntRect(0, 51, CHARGE_ANIMATION_FRAME_WIDTH, 32));
    window.draw(soundSprite);
    showSoundSprite = true;
}

void Client::handleSocketSend(const Direction dx, const Direction dy)
{
    const auto now = steady_clock::now();
    const auto duration = duration_cast<milliseconds>(now - lastSend).count();

    if ((duration > 16) && ((dx != NONE) || (dy != NONE) || (!lastSentNone))) {
        const PlayerEventMove event { dx, dy, clientId };
        auto buffer = BinaryProtocol::serializePlayerEventMove(event);
        network->send(buffer);
        lastSend = now;
        lastSentNone = ((dx == NONE) && (dy == NONE));
    }
    
    // Resend pending reliable events
    for (auto &pending : pendingEvents) {
        if (duration_cast<milliseconds>(now - pending.lastSent).count() >= 200) {
            auto buffer = BinaryProtocol::serializePlayerEvent(pending.event);
            network->send(buffer);
            pending.lastSent = now;
        }
    }
}

void Client::handleSocketReceive()
{
    vector<unsigned char> recvBuffer(65'504);
    size_t length = 0;
    network->setNonBlocking(true);
    while (true) {
        recvBuffer.resize(65'504);
        length = network->receive(recvBuffer);
        if (network->isWouldBlock() || length == 0) {
            break;
        }
        recvBuffer.resize(length);
            if (length > 0) {
            uint8_t eventType = recvBuffer[0];
            if (eventType == static_cast<uint8_t>(Event::SCORE_UPDATE)) {
                if (length >= sizeof(uint32_t)) {
                    uint32_t newScore;
                    memcpy(&newScore, &recvBuffer[1], sizeof(uint32_t));
                    updateScore(newScore);
                }
            } else if (eventType
                == static_cast<uint8_t>(Event::SPRITE_UPDATE)) {
                const auto receivedSprite
                    = BinaryProtocol::deserializeSpriteList(recvBuffer);
                for (const auto &sprite : receivedSprite) {
                    if (!sprites.contains(sprite.id)) {
                        sprites[sprite.id] = createSprite(sprite);
                        spriteStates[sprite.id].targetPosition = sprites[sprite.id].getPosition();
                    } else {
                        if (sprites[sprite.id].getTexture()
                            != &textures[sprite.spritesheetIndex]) {
                            sprites[sprite.id].setTexture(
                                textures[sprite.spritesheetIndex]);
                        }
                        sprites[sprite.id].setTextureRect(sf::IntRect(
                            sprite.x, sprite.y, sprite.width, sprite.height));
                        
                        // Setup interpolation and dead reckoning
                        sf::Vector2f newTarget = sf::Vector2f(
                            static_cast<float>(sprite.gameX),
                            static_cast<float>(sprite.gameY));
                        
                        spriteStates[sprite.id].velocity = newTarget - spriteStates[sprite.id].targetPosition;
                        spriteStates[sprite.id].targetPosition = newTarget;
                        
                        if (sprite.spritesheetIndex == 8) {
                            sprites[sprite.id].setRotation(
                                static_cast<float>(sprite.rotation));
                        }
                    }
                }
            } else if (length == 5 && eventType == static_cast<uint8_t>(Event::ACK)) {
                PacketAck ack = BinaryProtocol::deserializePacketAck(recvBuffer);
                uint32_t id = ack.packetId;
                auto it = std::remove_if(pendingEvents.begin(), pendingEvents.end(),
                    [id](const PendingEvent &e) { return e.event.packetId == id; });
                pendingEvents.erase(it, pendingEvents.end());
            } else if (length == 9) {
                const PlayerEvent event = BinaryProtocol::deserializePlayerEvent(recvBuffer);
                
                // Send ACK back for reliable events from server
                PacketAck ack { Event::ACK, event.packetId };
                auto ackBuffer = BinaryProtocol::serializePacketAck(ack);
                network->send(ackBuffer);
                
                if (event.event == Event::BOSS_FIGHT) {
                    isBossFight = (event.playerId == 1);
                    if (isBossFight) {
                        clientEngine->stopMusic("level1");
                        clientEngine->stopMusic("level2");
                        clientEngine->setMusicLoop("boss", true);
                        clientEngine->playMusic("boss");
                    }
                }
                if (event.event == Event::DESTROY) {
                    const auto it = sprites.find(event.playerId);
                    if (it != sprites.end()) {
                        const int sheetIndex = getSpriteSheetIndex(it->second);
                        if (((sheetIndex == 5) || (sheetIndex == 8)
                                || (sheetIndex == 6))
                            && (it->second.getPosition().x > 0)
                            && (it->second.getPosition().x < 1128)) {
                            clientEngine->playSound("kill");
                            
                            // Screen Shake and Particles
                            if (sheetIndex == 6) { // Boss Death
                                triggerScreenShake(1.0f, 6.0f); // Reduced duration and intensity
                                spawnExplosionParticles(it->second.getPosition(), 200, sf::Color(255, 100, 0, 255)); // Huge explosion
                            } else if (sheetIndex == 5) { // Mob Death
                                triggerScreenShake(0.08f, 1.5f); // Reduced duration and intensity
                                spawnExplosionParticles(it->second.getPosition(), 30, sf::Color(255, 200, 0, 255)); // Medium explosion
                            }
                        } else if (sheetIndex == 9) { // Player Death
                            triggerScreenShake(0.2f, 4.0f); // Reduced duration and intensity
                            spawnExplosionParticles(it->second.getPosition(), 50, sf::Color(0, 150, 255, 255)); // Blue explosion
                        }
                        sprites.erase(it);
                    }
                    int playerCount = 0;
                    for (const auto &[id, sprite] : sprites) {
                        if (getSpriteSheetIndex(sprite) == 9) {
                            playerCount++;
                        }
                    }
                    if (event.playerId == clientId && playerLives <= 0) {
                        isGameOver = true;
                        showGameOverScreen();
                    }
                }
            } else if (length == 12) {
                const PlayerEventLife event = BinaryProtocol::deserializePlayerEventLife(recvBuffer);
                
                PacketAck ack { Event::ACK, event.packetId };
                auto ackBuffer = BinaryProtocol::serializePacketAck(ack);
                network->send(ackBuffer);
                if (event.playerId == clientId) {
                    playerLives = event.lives;
                    if (playerLives <= 0) {
                        isGameOver = true;
                        showGameOverScreen();
                    }
                }
                playerLives = event.lives;
                clientEngine->playSound("death");
                if (event.lives <= 0) {
                    window.close();
                    auto buffer = BinaryProtocol::serializePlayerEvent(
                        { Event::DESTROY, clientId });
                    network->send(buffer);
                }
            } else if (length == 5 && eventType != static_cast<uint8_t>(Event::ACK)) {
                const PlayerEventLevel event = BinaryProtocol::deserializePlayerEventLevel(recvBuffer);
                
                PacketAck ack { Event::ACK, event.packetId };
                auto ackBuffer = BinaryProtocol::serializePacketAck(ack);
                network->send(ackBuffer);
                
                level = event.level;
                if (level == 3) {
                    isGameOver = true;
                    showGameOverScreen();
                }
                if (level == 2) {
                    currentBackgroundWidth = 534.0f;
                    backgroundSprite1.setTextureRect(sf::IntRect(505, 0,
                        static_cast<int>(currentBackgroundWidth), 224));
                    backgroundSprite2.setTextureRect(sf::IntRect(505, 0,
                        static_cast<int>(currentBackgroundWidth), 224));
                    backgroundSprite1.setScale(
                        1128.0f / currentBackgroundWidth, 672.0f / 224.0f);
                    backgroundSprite2.setScale(
                        1128.0f / currentBackgroundWidth, 672.0 / 224.0f);
                    clientEngine->stopMusic("level1");
                    clientEngine->stopMusic("boss");
                    clientEngine->setMusicLoop("level2", true);
                    clientEngine->playMusic("level2");
                } else {
                    clientEngine->stopMusic("level2");
                    clientEngine->stopMusic("boss");
                    clientEngine->playMusic("level1");
                    currentBackgroundWidth = 376;
                    backgroundSprite1.setTextureRect(sf::IntRect(
                        0, 0, static_cast<int>(currentBackgroundWidth), 224));
                    backgroundSprite2.setTextureRect(sf::IntRect(
                        0, 0, static_cast<int>(currentBackgroundWidth), 224));
                    backgroundSprite1.setScale(
                        1128.0f / currentBackgroundWidth, 672.0f / 224.0f);
                    backgroundSprite2.setScale(
                        1128.0f / currentBackgroundWidth, 672.0 / 224.0f);
                }
            } else {
                cerr << "Received unknown data of length " << length << "\n";
            }
        }
    }
}

void Client::interpolateSprites()
{
    for (auto& [id, sprite] : sprites) {
        if (!spriteStates.contains(id)) continue;
        auto& state = spriteStates[id];
        
        // Exponential smoothing (Lerp): Move a percentage of the way to the target every frame.
        // This makes movement smooth and naturally handles lag/jitter without freezing.
        sf::Vector2f currentPos = sprite.getPosition();
        sf::Vector2f newPos;
        float smoothingFactor = 0.4f; // 40% towards the target every frame
        
        newPos.x = currentPos.x + (state.targetPosition.x - currentPos.x) * smoothingFactor;
        newPos.y = currentPos.y + (state.targetPosition.y - currentPos.y) * smoothingFactor;
        
        sprite.setPosition(newPos);
    }
}

void Client::renderWindow()
{
    interpolateSprites();
    
    // Combo Logic
    if (comboTimer > 0.0f) {
        comboTimer -= 1.0f / 60.0f;
        // Scale lerp to 1.0f
        sf::Vector2f currentScale = comboText.getScale();
        if (currentScale.x > 1.0f) {
            comboText.setScale(currentScale.x - 0.05f, currentScale.y - 0.05f);
        }
        if (comboTimer <= 0.0f) {
            comboTimer = 0.0f;
            comboMultiplier = 1;
            comboText.setString("");
        }
    }
    
    // Screen Shake Logic
    if (screenShakeDuration > 0.0f) {
        float offsetX = (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 2.0f * screenShakeIntensity;
        float offsetY = (static_cast<float>(rand() % 100) / 100.0f - 0.5f) * 2.0f * screenShakeIntensity;
        
        mainView.setCenter(window.getSize().x / 2.0f + offsetX, window.getSize().y / 2.0f + offsetY);
        screenShakeDuration -= 1.0f / 60.0f; // assuming 60fps
        if (screenShakeDuration <= 0.0f) {
            mainView.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f); // Reset
            screenShakeDuration = 0.0f;
        }
    }
    window.setView(mainView);
    
    if (!isBossFight) {
        backgroundSprite1.move(static_cast<float>(-1 * level), 0.0f);
        backgroundSprite2.move(static_cast<float>(-1 * level), 0.0f);
    }
    if (backgroundSprite1.getPosition().x <= -1128.0f) {
        backgroundSprite1.setPosition(
            backgroundSprite2.getPosition().x + 1128.0f, 0.0f);
    }
    if (backgroundSprite2.getPosition().x <= -1128.0f) {
        backgroundSprite2.setPosition(
            backgroundSprite1.getPosition().x + 1128.0f, 0.0f);
    }
    if (playerLives <= 0 || isGameOver) {
        showGameOverScreen();
        return;
    }
    window.draw(backgroundSprite1);
    window.draw(backgroundSprite2);
    const auto stopCondition = sprites.rend();
    for (auto it = sprites.rbegin(); it != stopCondition; ++it) {
        if ((it->second.getTexture() == &textures[4]) && (!isBossFight)) {
            it->second.move(static_cast<float>(-1 * level), 0.0f);
        }
        window.draw(it->second);
    }
    if (!spectatorMode) {
        livesText.setString(format(
            "{} life{} left", playerLives, (playerLives > 1) ? "s" : ""));
    }
    window.draw(livesText);
    updateParticles();
    renderParticles();
    handleCollisions();
    updateSpriteColors();
    fadeIn();
    if (isCharging) {
        for (const auto &[id, sprite] : sprites) {
            if ((getSpriteSheetIndex(sprite) == 9) && (id == clientId)) {
                const auto chargeDuration = duration_cast<milliseconds>(
                    steady_clock::now() - chargeStartTime)
                                                .count();

                if (chargeDuration >= 500) {
                    if (chargeAnimationClock.getElapsedTime().asMilliseconds()
                        >= 64) {
                        chargeAnimationFrame = (chargeAnimationFrame + 1)
                            % CHARGE_ANIMATION_FRAMES;
                        chargeAnimationClock.restart();
                        chargeIndicator.setTextureRect(
                            sf::IntRect(chargeAnimationFrame
                                    * CHARGE_ANIMATION_FRAME_WIDTH,
                                51, CHARGE_ANIMATION_FRAME_WIDTH, 32));
                    }
                    chargeIndicator.setPosition(
                        (sprite.getPosition().x
                            + sprite.getGlobalBounds().width),
                        (sprite.getPosition().y
                            + ((sprite.getGlobalBounds().height / 2)
                                - (chargeIndicator.getGlobalBounds().height
                                    / 2))));
                    window.draw(chargeIndicator);
                } else {
                    normalShotDisplayed = true;
                    normalShotTimer.restart();
                    chargeIndicator.setTextureRect(
                        sf::IntRect(215, 85, 14, 12));
                    chargeIndicator.setPosition(
                        (sprite.getPosition().x
                            + sprite.getGlobalBounds().width),
                        (sprite.getPosition().y
                            + ((sprite.getGlobalBounds().height / 2)
                                - (chargeIndicator.getGlobalBounds().height
                                    / 2))));
                    window.draw(chargeIndicator);
                }
                break;
            }
        }
    } else if (normalShotDisplayed
        && (normalShotTimer.getElapsedTime().asMilliseconds() < 32)) {
        for (const auto &[id, sprite] : sprites) {
            if ((getSpriteSheetIndex(sprite) == 9) && (id == clientId)) {
                chargeIndicator.setTextureRect(sf::IntRect(215, 85, 14, 12));
                chargeIndicator.setPosition(
                    ((sprite.getPosition().x)
                        + (sprite.getGlobalBounds().width) + 5),
                    ((sprite.getPosition().y)
                        + ((sprite.getGlobalBounds().height / 2) - 4.5f)));
                window.draw(chargeIndicator);
                break;
            }
        }
    } else {
        normalShotDisplayed = false;
    }
    if (showSoundSprite) {
        window.draw(soundSprite);
        showSoundSprite = false;
    }
    window.draw(scoreText);
    if (comboMultiplier > 1) {
        window.draw(comboText);
    }
    lagText.setString("Lag " + to_string(duration) + " ms");
    window.draw(lagText);
    window.display();
}

void Client::run()
{
    handleSocketOpen();
    if (!spectatorMode) {
        network->send(connectBuffer);
    } else {
        cout << "Entering spectator mode\n";
        auto spectatorBuffer
            = BinaryProtocol::serializePlayerEvent({ Event::SPECTATOR, 0 });
        network->send(spectatorBuffer);
    }
    lastSend = steady_clock::now();
    auto lastRenderTime = steady_clock::now();
    if (!font.loadFromFile("src/client/assets/r-type.ttf")) {
        cerr << "Failed to load font\n";
        ::exit(84);
    }
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(10, 10);
    livesText.setString("Spectator mode");

    mt19937 gen(random_device {}());
    uniform_real_distribution<float> distX(-0.5f, -0.2f);
    uniform_real_distribution<float> distY(-1.0f, 1.0f);
    uniform_int_distribution distLifespan(50, 100);
    uniform_real_distribution<float> distSize(5.0f, 10.0f);
    fadeRect = sf::RectangleShape(sf::Vector2f(window.getSize()));
    fadeRect.setFillColor(sf::Color(0, 0, 0, 255));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                fadeOut();
                window.close();
                if (!spectatorMode) {
                    auto buffer = BinaryProtocol::serializePlayerEvent(
                        { Event::DESTROY, clientId });
                    network->send(buffer);
                }
            }
        }

        if (!clientIdReceived && !spectatorMode) {
            handleClientIdReception();
            continue;
        }

        Direction dx = NONE;
        Direction dy = NONE;

        if (!spectatorMode) {
            handlePlayerInput(dx, dy);
            handlePlayerInput(dx, dy);
            handlePlayerInput(dx, dy);
            handleSocketSend(dx, dy);
        }

        handleSocketReceive();

        const auto now = steady_clock::now();
        duration = duration_cast<milliseconds>(now - lastRenderTime).count();
        if (duration >= 16) {
            for (const auto &[id, sprite] : sprites) {
                if (sprite.getTexture() == &textures[9]) {
                    Particle particle;
                    const float size = distSize(gen);
                    particle.shape.setSize(sf::Vector2f(size, size));
                    particle.shape.setFillColor(sf::Color(0, 0, 255, 255));

                    sf::Vector2f particlePosition = sprite.getPosition();
                    particlePosition.y += 12.0f;
                    particlePosition.x -= size;
                    particle.shape.setPosition(particlePosition);

                    particle.velocity = sf::Vector2f(distX(gen), distY(gen));
                    particle.lifespan = distLifespan(gen);
                    particle.initialSize = size;
                    particles.emplace_back(particle);
                }
            }
            renderWindow();
            lastRenderTime = now;
        }
        network->poll();
    }
}

void Client::fadeOut()
{
    fadeRect.setFillColor(sf::Color(0, 0, 0, 0));
    while (true) {
        fadeRect.setFillColor(sf::Color(0, 0, 0, fadeAlpha));
        window.draw(fadeRect);
        window.display();
        sf::sleep(sf::milliseconds(10));
        fadeAlpha += 3;
        if (fadeAlpha == 255) {
            return;
        }
    }
}

void Client::fadeIn()
{
    if (fadeAlpha > 0) {
        fadeRect.setFillColor(sf::Color(0, 0, 0, fadeAlpha));
        window.draw(fadeRect);
        fadeAlpha -= 3;
    }
}

int Client::getSpriteSheetIndex(const sf::Sprite &sprite) const
{
    for (unsigned int i = 1; i <= spritesNumber; ++i) {
        if (sprite.getTexture() == &textures.at(i)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void Client::handleCollisions()
{
    for (const auto &[id1, sprite1] : sprites) {
        const int sheetIndex1 = getSpriteSheetIndex(sprite1);
        if (sheetIndex1 == 1) {
            for (const auto &[id2, sprite2] : sprites) {
                const int sheetIndex2 = getSpriteSheetIndex(sprite2);
                if ((sheetIndex2 == 2) && checkCollision(sprite1, sprite2)) {
                    auto &alreadyCollided = collisionsMap[id2];
                    if (!alreadyCollided.contains(id1)) {
                        alreadyCollided.insert(id1);
                        auto &state = spriteStates[id2];
                        state.isInCollision = true;
                        state.originalColor = sprite2.getColor();
                        state.collisionTimer.restart();
                        sprites[id2].setColor(sf::Color(100, 100, 255, 255));
                    }
                }
            }
        }
    }
}

bool Client::checkCollision(
    const sf::Sprite &sprite1, const sf::Sprite &sprite2) const
{
    const sf::FloatRect bounds1 = sprite1.getGlobalBounds();
    sf::FloatRect bounds2 = sprite2.getGlobalBounds();
    bounds2.left -= 10;

    return bounds1.intersects(bounds2);
}

void Client::updateSpriteColors()
{
    for (auto &[id, state] : spriteStates) {
        if (state.isInCollision
            && (state.collisionTimer.getElapsedTime().asSeconds() >= 0.1f)) {
            sprites[id].setColor(state.originalColor);
            state.isInCollision = false;
        }
    }
}
