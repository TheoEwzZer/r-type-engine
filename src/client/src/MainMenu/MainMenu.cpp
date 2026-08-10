/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** MainMenu.cpp
*/

#include "MainMenu.hpp"
#include "src/EditorEngine/EditorEngine.hpp"
#include <Components/Slider/Slider.hpp>
#include <Components/Textinput/Textinput.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

using enum TextSize;

MainMenu::MainMenu(ClientEngine &engine, Client &client,
    const shared_ptr<EditorEngine> editor) :
    _clientEngine(engine),
    _client(client), _window(engine.getWindow()),
    previousShootButtonState(false), _editor(editor)
{
    if (!_titleFont.loadFromFile(
            "./src/client/assets/fonts/Montserrat/Montserrat-Bold.ttf")) {
        throw MainMenuException("Could not load title font");
    }
    if (!buttonFont.loadFromFile(
            "src/client/assets/fonts/Montserrat/Montserrat-Thin.ttf")) {
        throw MainMenuException("Could not load button font");
    }
    cursorPosition
        = sf::Vector2f(static_cast<float>(_window.getSize().x) / 2.0f,
            static_cast<float>(_window.getSize().y) / 2.0f);
    initializePlayModal();
    usingController = false;
}

void MainMenu::initialize()
{
    initializeComponents();
    updateControlsFromSettings();
    masterVolumeSlider->setValue(_clientEngine.getMasterVolume());
    _musicVolumeSlider->setValue(_clientEngine.getMusicVolume());
    effectsVolumeSlider->setValue(_clientEngine.getEffectsVolume());
}

void MainMenu::initializeFloatingSprites()
{
    struct SpriteInfo {
        string texturePath;
        sf::IntRect textureRect;
        float scale;
    };

    const vector<SpriteInfo> spriteTypes
        = { { "src/client/assets/r-typesheet8.png", sf::IntRect(7, 40, 22, 22),
                2.0f },
              { "src/client/assets/r-typesheet5.png",
                  sf::IntRect(0, 0, 21, 24), 2.0f },
              { "src/client/assets/r-typesheet3.png",
                  sf::IntRect(0, 0, 176, 176), 0.5f },
              { "src/client/assets/r-typesheet9.png",
                  sf::IntRect(67, 3, 32, 12), 2.0f },
              { "src/client/assets/r-typesheet9.png",
                  sf::IntRect(67, 20, 32, 12), 2.0f },
              { "src/client/assets/r-typesheet9.png",
                  sf::IntRect(67, 37, 32, 12), 2.0f },
              { "src/client/assets/r-typesheet9.png",
                  sf::IntRect(67, 54, 32, 12), 2.0f } };

    const vector<pair<int, int>> positions = { { 900, 336 }, { 100, 100 },
        { 900, 150 }, { 200, 400 }, { 800, 500 }, { 500, 300 }, { 300, 200 },
        { 700, 350 }, { 150, 250 }, { 850, 450 } };

    _textures.clear();
    _floatingSprites.clear();

    for (const auto &spriteInfo : spriteTypes) {
        sf::Texture texture;
        if (!texture.loadFromFile(spriteInfo.texturePath)) {
            throw MainMenuException(
                "Could not load texture: " + spriteInfo.texturePath);
        }
        _textures.push_back(texture);
    }

    const size_t positionCount = positions.size();
    const size_t spriteTypeCount = spriteTypes.size();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 359);

    for (size_t i = 0; i < positionCount; ++i) {
        const auto &spriteInfo = spriteTypes[i % spriteTypeCount];

        sf::Sprite sprite;
        sprite.setTexture(_textures[i % spriteTypeCount]);
        sprite.setTextureRect(spriteInfo.textureRect);
        sprite.setScale(spriteInfo.scale, spriteInfo.scale);
        sprite.setPosition(static_cast<float>(positions[i].first),
            static_cast<float>(positions[i].second));

        if (i > 0) {
            sprite.setRotation(dis(gen));
        }

        sf::Color color = sprite.getColor();
        color.a = 128;
        sprite.setColor(color);

        _floatingSprites.push_back(sprite);
    }
}

void MainMenu::updateFloatingSprites() const
{
    _backgroundSprite1.move(-1.0f, 0);
    _backgroundSprite2.move(-1.0f, 0);

    if (_backgroundSprite1.getPosition().x <= -1128.0f) {
        _backgroundSprite1.setPosition(
            _backgroundSprite2.getPosition().x + 1128.0f, 0);
    }
    if (_backgroundSprite2.getPosition().x <= -1128.0f) {
        _backgroundSprite2.setPosition(
            _backgroundSprite1.getPosition().x + 1128.0f, 0);
    }

    static float elapsedTime = 0.0f;
    elapsedTime += 0.016f;

    for (auto &sprite : _floatingSprites) {
        const float xOffset
            = sin(elapsedTime + sprite.getPosition().x * 0.01f) * 0.5f;
        const float yOffset
            = cos(elapsedTime + sprite.getPosition().y * 0.01f) * 0.5f;

        sprite.move(xOffset, yOffset);
        sprite.rotate(0.1f);
        sf::Vector2f pos = sprite.getPosition();
        if (pos.x < -50) {
            pos.x = 1178;
        }
        if (pos.x > 1178) {
            pos.x = -50;
        }
        if (pos.y < -50) {
            pos.y = 722;
        }
        if (pos.y > 722) {
            pos.y = -50;
        }
        sprite.setPosition(pos);
    }
}

void MainMenu::drawFloatingSprites(sf::RenderWindow &window) const
{
    window.draw(_backgroundSprite1);
    window.draw(_backgroundSprite2);
    for (const auto &sprite : _floatingSprites) {
        window.draw(sprite);
    }
}

void MainMenu::initializeComponents()
{
    if (!_backgroundTexture.loadFromFile(
            "src/client/assets/r-typesheet4.png")) {
        throw MainMenuException("Could not load background texture");
    }
    _backgroundSprite1.setTexture(_backgroundTexture);
    _backgroundSprite2.setTexture(_backgroundTexture);
    _backgroundSprite1.setTextureRect(sf::IntRect(0, 0, 376, 224));
    _backgroundSprite2.setTextureRect(sf::IntRect(0, 0, 376, 224));
    _backgroundSprite1.setScale(1128.0f / 376.0f, 672.0f / 224.0f);
    _backgroundSprite2.setScale(1128.0f / 376.0f, 672.0f / 224.0f);
    _backgroundSprite1.setPosition(0, 0);
    _backgroundSprite2.setPosition(1128, 0);

    if (!_logoTexture.loadFromFile("src/client/assets/R-Type_logo.png")) {
        throw MainMenuException("Could not load logo texture");
    }
    _logoSprite.setTexture(_logoTexture);
    _logoSprite.setScale(0.7f, 0.7f);
    _logoSprite.setPosition(
        (1128.0f - _logoSprite.getGlobalBounds().width) / 2.0f + 20.0f, 50.0f);
    initializeFloatingSprites();
    auto playButton
        = make_unique<Button>(464, 250, 200, 50, "Play", buttonFont);
    auto settingsButton
        = make_unique<Button>(464, 350, 200, 50, "Settings", buttonFont);
    auto exitButton
        = make_unique<Button>(464, 450, 200, 50, "Exit", buttonFont);
    auto editorButton
        = make_unique<Button>(464, 550, 200, 50, "Editor", buttonFont);

    _components.push_back(move(playButton));
    _components.push_back(move(settingsButton));
    _components.push_back(move(exitButton));
    _components.push_back(move(editorButton));

    settingsModal = make_unique<Modal>(
        sf::Vector2f(800, 500), buttonFont, string("Settings"));
    settingsModal->setPosition(164, 86);

    overlay.setSize(sf::Vector2f(1128, 672));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    controlTypeSelect = make_unique<Select>(
        sf::Vector2f(settingsModal->getContent().getPosition().x + 150,
            settingsModal->getContent().getPosition().y + 20),
        sf::Vector2f(200, 40), buttonFont, "Control Type");
    controlTypeSelect->addOption("Keyboard");
    controlTypeSelect->addOption("Controller");

    initializeTextFields();
    initializeVolumeControls();
    initializeKeybindings();
    initializeTabs();
    checkControllerConnection();

    controlTypeSelect->setCallback(
        [this, shootBinding = shootBinding.get(), upBinding = upBinding.get(),
            downBinding = downBinding.get(), leftBinding = leftBinding.get(),
            rightBinding = rightBinding.get(),
            controllerShootBinding = controllerShootBinding.get(),
            stickSelect = stickSelect.get()](string_view option) {
            if (!(shootBinding != nullptr && upBinding != nullptr
                    && downBinding != nullptr && leftBinding != nullptr
                    && rightBinding != nullptr
                    && controllerShootBinding != nullptr
                    && stickSelect != nullptr)) {
                return;
            }

            if (option == "Keyboard") {
                shootBinding->show();
                upBinding->show();
                downBinding->show();
                leftBinding->show();
                rightBinding->show();

                if (controllerShootBinding != nullptr) {
                    controllerShootBinding->hide();
                }
                if (stickSelect != nullptr) {
                    stickSelect->hide();
                }
            } else if (option == "Controller") {
                if (!hasController) {
                    controlTypeSelect->setSelectedOption("Keyboard");
                    return;
                }
                shootBinding->hide();
                upBinding->hide();
                downBinding->hide();
                leftBinding->hide();
                rightBinding->hide();

                if (controllerShootBinding != nullptr) {
                    controllerShootBinding->show();
                }
                if (stickSelect != nullptr) {
                    stickSelect->show();
                }
            }
        });
    settingsModal->setSaveCallback([this]() { saveSettings(); });

    masterVolumeSlider->setCallback([this](const float value) {
        _clientEngine.setMasterVolume(value);
        settingsModal->setPendingChanges(true);
        settingsModal->initializeSaveButton();
    });

    _musicVolumeSlider->setCallback([this](const float value) {
        _clientEngine.setMusicVolume(value);
        settingsModal->setPendingChanges(true);
        settingsModal->initializeSaveButton();
    });

    effectsVolumeSlider->setCallback([this](const float value) {
        _clientEngine.setEffectsVolume(value);
        settingsModal->setPendingChanges(true);
        settingsModal->initializeSaveButton();
    });

    auto &inputs = _clientEngine.getInputManager();
    shootBinding->setCallback([this, &inputs](sf::Keyboard::Key key) {
        inputs.setShootKey(key);
        settingsModal->setPendingChanges(true);
    });

    upBinding->setCallback([this, &inputs](sf::Keyboard::Key key) {
        inputs.setUpKey(key);
        settingsModal->setPendingChanges(true);
    });

    downBinding->setCallback([this, &inputs](sf::Keyboard::Key key) {
        inputs.setDownKey(key);
        settingsModal->setPendingChanges(true);
    });

    leftBinding->setCallback([this, &inputs](sf::Keyboard::Key key) {
        inputs.setLeftKey(key);
        settingsModal->setPendingChanges(true);
    });

    rightBinding->setCallback([this, &inputs](sf::Keyboard::Key key) {
        inputs.setRightKey(key);
        settingsModal->setPendingChanges(true);
    });

    controllerShootBinding->setJoystickButtonCallback(
        [this, &inputs](unsigned int button) {
            inputs.setControllerShootButton(button);
            settingsModal->setPendingChanges(true);
        });

    invertXAxis->setCallback([this, &inputs](bool checked) {
        inputs.setInvertXAxis(checked);
        settingsModal->setPendingChanges(true);
    });

    invertYAxis->setCallback([this, &inputs](bool checked) {
        inputs.setInvertYAxis(checked);
        settingsModal->setPendingChanges(true);
    });

    updateControlsFromSettings();
    settingsModal->initializeSaveButton();

    textSizeSelect->setCallback(
        [this](const string &option) { handleTextSizeChange(option); });

    playModal->addComponent(_playTextInput.get());
    playModal->addButton(
        *validateButton, [this]() { handleValidateButton(); });
}

void MainMenu::updateControlsFromSettings()
{
    const auto &inputs = _clientEngine.getInputManager();

    masterVolumeSlider->setValue(_clientEngine.getMasterVolume());
    _musicVolumeSlider->setValue(_clientEngine.getMusicVolume());
    effectsVolumeSlider->setValue(_clientEngine.getEffectsVolume());
    shootBinding->setKey(inputs.getShootKey());
    upBinding->setKey(inputs.getUpKey());
    downBinding->setKey(inputs.getDownKey());
    leftBinding->setKey(inputs.getLeftKey());
    rightBinding->setKey(inputs.getRightKey());
    controllerShootBinding->setJoystickButton(
        inputs.getControllerShootButton());
    invertXAxis->setChecked(inputs.getInvertXAxis());
    invertYAxis->setChecked(inputs.getInvertYAxis());
}

void MainMenu::render(sf::RenderWindow &window) const
{
    window.clear(sf::Color(50, 50, 50));
    updateFloatingSprites();
    drawFloatingSprites(window);
    window.draw(_logoSprite);
    for (const auto &component : _components) {
        component->draw(window);
    }

    if (settingsModal && settingsModal->isVisible()) {
        window.draw(overlay);
        settingsModal->draw(window);
    }

    if (playModal && playModal->isVisible()) {
        window.draw(overlay);
        playModal->draw(window);
    }

    window.display();
}

void MainMenu::handleEvents(sf::RenderWindow &window)
{
    handleControllerCursor(window);

    sf::Event event;
    while (window.pollEvent(event)) {
        if (editorActive_) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            continue;
        }
        if (handleGlobalEvents(event)) {
            continue;
        }

        if (settingsModal && settingsModal->isVisible()) {
            handleSettingsModalEvents(event, window);
            continue;
        }

        if (playModal && playModal->isVisible()) {
            handlePlayModalEvents(event);
            continue;
        }
        if (event.type == sf::Event::MouseMoved) {
            usingController = false;
            cursorPosition
                = sf::Vector2f(static_cast<float>(event.mouseMove.x),
                    static_cast<float>(event.mouseMove.y));
        }
        if (event.type == sf::Event::JoystickMoved) {
            usingController = true;
        }

        handleComponentEvents(event, window);
    }
    if (usingController) {
        handleControllerCursor(window);
    }
}

void MainMenu::handleControllerCursor(sf::RenderWindow &window)
{
    if (!hasController) {
        return;
    }
    const auto &inputs = _clientEngine.getInputManager();

    const float xAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
    const float yAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    const bool shootPressed
        = sf::Joystick::isButtonPressed(0, inputs.getControllerShootButton());

    const float deadzone = 20.0f;

    if ((abs(xAxis) > deadzone) || (abs(yAxis) > deadzone)) {
        if (abs(xAxis) > deadzone) {
            cursorPosition.x += (xAxis / 100.0f) * _cursorSpeed;
        }
        if (abs(yAxis) > deadzone) {
            cursorPosition.y += (yAxis / 100.0f) * _cursorSpeed;
        }

        cursorPosition.x = max(0.0f,
            min(cursorPosition.x, static_cast<float>(window.getSize().x)));
        cursorPosition.y = max(0.0f,
            min(cursorPosition.y, static_cast<float>(window.getSize().y)));

        sf::Mouse::setPosition(sf::Vector2i(cursorPosition), window);
    }

    if (shootPressed && (!previousShootButtonState)) {
        sf::Event event;
        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.button = sf::Mouse::Left;
        event.mouseButton.x = static_cast<int>(cursorPosition.x);
        event.mouseButton.y = static_cast<int>(cursorPosition.y);
        handleComponentEvents(event, window);

        if (settingsModal && settingsModal->isVisible()) {
            handleSettingsModalEvents(event, window);
        }

        if (playModal && playModal->isVisible()) {
            handlePlayModalEvents(event);
        }
    }

    previousShootButtonState = shootPressed;
}

void MainMenu::handleSettingsModalEvents(
    const sf::Event &event, const sf::RenderWindow &window)
{
    settingsModal->handleEvent(event);

    if (masterVolumeSlider && _musicVolumeSlider && effectsVolumeSlider) {
        masterVolumeSlider->handleEvent(event, window);
        _musicVolumeSlider->handleEvent(event, window);
        effectsVolumeSlider->handleEvent(event, window);
    }

    if (controlTypeSelect) {
        controlTypeSelect->handleEvent(event, window);
    }
    if (stickSelect) {
        stickSelect->handleEvent(event, window);
    }
    if (textSizeSelect) {
        textSizeSelect->handleEvent(event, window);
    }

    if (shootBinding) {
        shootBinding->handleEvent(event);
    }
    if (upBinding) {
        upBinding->handleEvent(event);
    }
    if (downBinding) {
        downBinding->handleEvent(event);
    }
    if (leftBinding) {
        leftBinding->handleEvent(event);
    }
    if (rightBinding) {
        rightBinding->handleEvent(event);
    }

    if (controllerShootBinding) {
        controllerShootBinding->handleEvent(event);
    }
    if (controllerBackBinding) {
        controllerBackBinding->handleEvent(event);
    }

    if (invertXAxis) {
        invertXAxis->handleEvent(event, window);
    }
    if (invertYAxis) {
        invertYAxis->handleEvent(event, window);
    }
}

void MainMenu::handlePlayModalEvents(const sf::Event &event)
{
    playModal->handleEvent(event);
    if (_playTextInput) {
        _playTextInput->handleEvent(event);
    }
    if (validateButton) {
        validateButton->handleEvent(event);
    }
}

bool MainMenu::handleGlobalEvents(const sf::Event &event) const
{
    if ((event.type == sf::Event::Closed)
        || (event.type == sf::Event::KeyPressed
            && (event.key.code == sf::Keyboard::Escape))) {
        ::exit(0);
    }
    return false;
}

void MainMenu::handleComponentEvents(
    const sf::Event &event, sf::RenderWindow &window)
{
    for (const auto &component : _components) {
        if (const auto *button
            = dynamic_cast<const Button *>(component.get())) {
            if (button->isClicked(window, event)) {
                handleButtonClick(button->getText());
            }
        }
    }
}

void MainMenu::handleButtonClick(const string_view buttonText)
{
    if (buttonText == "Exit") {
        ::exit(0);
    } else if (buttonText == "Play") {
        _client.run();
    } else if (buttonText == "Settings") {
        settingsModal->show();
    } else if (buttonText == "Editor") {
        editorActive_ = true;
    }
}

bool MainMenu::menuLoop(sf::RenderWindow &window)
{
    bool goEditor = false;
    initialize();
    while (window.isOpen()) {
        handleEvents(window);
        if (editorActive_) {
            goEditor = true;
            break;
        }
        render(window);
    }
    return goEditor;
}

void MainMenu::updateKeybindDisplays()
{
    const auto &inputs = _clientEngine.getInputManager();
    shootBinding->setKey(inputs.getShootKey());
    upBinding->setKey(inputs.getUpKey());
    downBinding->setKey(inputs.getDownKey());
    leftBinding->setKey(inputs.getLeftKey());
    rightBinding->setKey(inputs.getRightKey());
}

void MainMenu::checkControllerConnection()
{
    hasController = sf::Joystick::isConnected(0);
}

void MainMenu::toggleController()
{
    hasController = !hasController;

    if ((!hasController)
        && (controlTypeSelect->getSelectedOption() == "Controller")) {
        controlTypeSelect->setSelectedOption("Keyboard");
    }

    controlTypeSelect->addOption("Keyboard");
    if (hasController) {
        controlTypeSelect->addOption("Controller");
    }

    controlTypeSelect->setCallback([this](const string &option) {
        if (option == "Keyboard") {
            shootBinding->show();
            upBinding->show();
            downBinding->show();
            leftBinding->show();
            rightBinding->show();
        } else if (option == "Controller") {
            shootBinding->hide();
            upBinding->hide();
            downBinding->hide();
            leftBinding->hide();
            rightBinding->hide();
        }
    });
}

void MainMenu::initializeTextFields()
{
    _currentTextSize = TextSize::MEDIUM;

    textSizeSelect = make_unique<Select>(sf::Vector2f(390, 300),
        sf::Vector2f(200, 40), buttonFont, "Text Size");
    textSizeSelect->addOption("Small");
    textSizeSelect->addOption("Medium");
    textSizeSelect->addOption("Medium or More");
    textSizeSelect->addOption("Large");

    textSizeSelect->setCallback([this](string_view option) {
        using enum TextSize;
        const TextSize newSize = (option == "Small") ? SMALL
            : (option == "Large")                    ? LARGE
            : (option == "Medium or More")           ? MEDIUM_OR_MORE
                                                     : MEDIUM;

        updateAllTextSizes(newSize);
    });

    volumeText = make_unique<TextField>(
        sf::Vector2f(settingsModal->getContent().getPosition().x + 20,
            settingsModal->getContent().getPosition().y + 20),
        buttonFont, TextStyle::SETTINGS_LABEL, "Adjust Volume Levels");

    masterLabel = make_unique<TextField>(sf::Vector2f(790, 190), buttonFont,
        TextStyle::SETTINGS_LABEL, "Master Volume");

    musicLabel = make_unique<TextField>(sf::Vector2f(790, 290), buttonFont,
        TextStyle::SETTINGS_LABEL, "Music Volume");

    effectsLabel = make_unique<TextField>(sf::Vector2f(790, 390), buttonFont,
        TextStyle::SETTINGS_LABEL, "Effects Volume");

    graphicsText = make_unique<TextField>(
        sf::Vector2f(settingsModal->getContent().getPosition().x + 20,
            settingsModal->getContent().getPosition().y + 20),
        buttonFont, TextStyle::SUBTITLE, "Graphics Settings");

    colorblindText = make_unique<TextField>(sf::Vector2f(390, 260), buttonFont,
        TextStyle::SETTINGS_LABEL, "Text size");

    controllerWarning = make_unique<TextField>(
        sf::Vector2f(controlTypeSelect->getPosition().x + 250,
            controlTypeSelect->getPosition().y + 10),
        buttonFont, TextStyle::WARNING_TEXT, "No controller connected!",
        sf::Color::Red);
}

void MainMenu::initializeVolumeControls()
{
    masterVolumeSlider
        = make_unique<Slider>(sf::Vector2f(264, 190), sf::Vector2f(400, 20));
    _musicVolumeSlider
        = make_unique<Slider>(sf::Vector2f(264, 290), sf::Vector2f(400, 20));
    effectsVolumeSlider
        = make_unique<Slider>(sf::Vector2f(264, 390), sf::Vector2f(400, 20));

    masterMuteButton = make_unique<Button>(214, 190, 40, 20, "M", buttonFont);
    musicMuteButton = make_unique<Button>(214, 290, 40, 20, "M", buttonFont);
    effectsMuteButton = make_unique<Button>(214, 390, 40, 20, "M", buttonFont);

    masterVolumeSlider->setValue(_clientEngine.getMasterVolume());
    _musicVolumeSlider->setValue(_clientEngine.getMusicVolume());
    effectsVolumeSlider->setValue(_clientEngine.getEffectsVolume());
}

void MainMenu::initializeKeybindings()
{
    auto &inputs = _clientEngine.getInputManager();
    shootBinding = make_unique<KeybindInput>(
        sf::Vector2f(350, 330), sf::Vector2f(200, 40), buttonFont, "Shoot");
    upBinding = make_unique<KeybindInput>(
        sf::Vector2f(350, 380), sf::Vector2f(200, 40), buttonFont, "Up");
    downBinding = make_unique<KeybindInput>(
        sf::Vector2f(350, 430), sf::Vector2f(200, 40), buttonFont, "Down");
    leftBinding = make_unique<KeybindInput>(
        sf::Vector2f(350, 480), sf::Vector2f(200, 40), buttonFont, "Left");
    rightBinding = make_unique<KeybindInput>(
        sf::Vector2f(350, 530), sf::Vector2f(200, 40), buttonFont, "Right");

    controllerShootBinding = make_unique<KeybindInput>(sf::Vector2f(1200, 440),
        sf::Vector2f(200, 40), buttonFont, "Shoot Button",
        InputType::ControllerButton);

    controllerBackBinding = make_unique<KeybindInput>(sf::Vector2f(1200, 490),
        sf::Vector2f(200, 40), buttonFont, "Back Button",
        InputType::ControllerButton);
    controllerBackBinding->setJoystickButtonCallback(
        [this, &inputs](unsigned int button) {
            inputs.setControllerBackButton(button);
            settingsModal->setPendingChanges(true);
        });

    invertXAxis = make_unique<Checkbox>(
        sf::Vector2f(960, 540), 20, buttonFont, "Invert X");
    invertYAxis = make_unique<Checkbox>(
        sf::Vector2f(1100, 540), 20, buttonFont, "Invert Y");

    invertXAxis->setCallback([this, &inputs](bool checked) {
        inputs.setInvertXAxis(checked);
        settingsModal->setPendingChanges(true);
    });

    invertYAxis->setCallback([this, &inputs](bool checked) {
        inputs.setInvertYAxis(checked);
        settingsModal->setPendingChanges(true);
    });

    shootBinding->setKey(inputs.getShootKey());
    upBinding->setKey(inputs.getUpKey());
    downBinding->setKey(inputs.getDownKey());
    leftBinding->setKey(inputs.getLeftKey());
    rightBinding->setKey(inputs.getRightKey());

    controllerShootBinding->setJoystickButton(
        inputs.getControllerShootButton());

    shootBinding->setCallback(
        [&inputs](const sf::Keyboard::Key key) { inputs.setShootKey(key); });
    upBinding->setCallback(
        [&inputs](const sf::Keyboard::Key key) { inputs.setUpKey(key); });
    downBinding->setCallback(
        [&inputs](const sf::Keyboard::Key key) { inputs.setDownKey(key); });
    leftBinding->setCallback(
        [&inputs](const sf::Keyboard::Key key) { inputs.setLeftKey(key); });
    rightBinding->setCallback(
        [&inputs](const sf::Keyboard::Key key) { inputs.setRightKey(key); });

    controllerShootBinding->setJoystickButtonCallback(
        [&inputs](const unsigned int button) {
            inputs.setControllerShootButton(button);
        });

    invertXAxis->setCallback(
        [&inputs](bool checked) { inputs.setInvertXAxis(checked); });
    invertYAxis->setCallback(
        [&inputs](bool checked) { inputs.setInvertYAxis(checked); });

    stickSelect = make_unique<Select>(sf::Vector2f(310, 350),
        sf::Vector2f(200, 40), buttonFont, "Stick Selection");
    stickSelect->addOption("Left Stick");
    stickSelect->addOption("Right Stick");

    controllerShootBinding = make_unique<KeybindInput>(sf::Vector2f(310, 300),
        sf::Vector2f(200, 40), buttonFont, "Shoot Button",
        InputType::ControllerButton);

    invertXAxis = make_unique<Checkbox>(
        sf::Vector2f(600, 440), 20, buttonFont, "Invert X");
    invertYAxis = make_unique<Checkbox>(
        sf::Vector2f(600, 470), 20, buttonFont, "Invert Y");
}

void MainMenu::initializeTabs()
{
    int startX = 100;
    int y = 50;
    const int TAB_WIDTH = 100;
    const int TAB_HEIGHT = 30;
    string tabName = "New Tab";

    if (!settingsModal) {
        return;
    }

    settingsModal->addTab("Volume", [&](sf::RenderWindow &window) {
        if (volumeText) {
            volumeText->draw(window);
        }
        if (masterLabel) {
            masterLabel->draw(window);
        }
        if (musicLabel) {
            musicLabel->draw(window);
        }
        if (effectsLabel) {
            effectsLabel->draw(window);
        }

        if (masterVolumeSlider) {
            masterVolumeSlider->draw(window);
        }
        if (_musicVolumeSlider) {
            _musicVolumeSlider->draw(window);
        }
        if (effectsVolumeSlider) {
            effectsVolumeSlider->draw(window);
        }

        if (masterMuteButton) {
            masterMuteButton->draw(window);
        }
        if (musicMuteButton) {
            musicMuteButton->draw(window);
        }
        if (effectsMuteButton) {
            effectsMuteButton->draw(window);
        }
    });

    settingsModal->selectTab(0);

    settingsModal->addTab("Binds", [&](sf::RenderWindow &window) {
        if (controlTypeSelect) {
            controlTypeSelect->draw(window);
        }

        if ((!hasController) && controllerWarning) {
            controllerWarning->draw(window);
        }

        if (controlTypeSelect
            && (controlTypeSelect->getSelectedOption() == "Keyboard")) {
            if (shootBinding) {
                shootBinding->draw(window);
            }
            if (upBinding) {
                upBinding->draw(window);
            }
            if (downBinding) {
                downBinding->draw(window);
            }
            if (leftBinding) {
                leftBinding->draw(window);
            }
            if (rightBinding) {
                rightBinding->draw(window);
            }
        } else if (controlTypeSelect
            && (controlTypeSelect->getSelectedOption() == "Controller")) {
            if (controllerTitle) {
                controllerTitle->draw(window);
            }
            if (moveLabel) {
                moveLabel->draw(window);
            }
            if (stickSelect) {
                stickSelect->draw(window);
            }
            if (shootLabel) {
                shootLabel->draw(window);
            }
            if (controllerShootBinding) {
                controllerShootBinding->draw(window);
            }
            if (invertXAxis) {
                invertXAxis->draw(window);
            }
            if (invertYAxis) {
                invertYAxis->draw(window);
            }
        }
    });

    settingsModal->addTab("Graphics", [&](sf::RenderWindow &window) {
        if (graphicsText) {
            graphicsText->draw(window);
        }
        if (colorblindText) {
            colorblindText->draw(window);
        }
        if (textSizeSelect) {
            textSizeSelect->draw(window);
        }
    });

    const auto tabButton = make_unique<Button>(
        startX, y, TAB_WIDTH, TAB_HEIGHT, tabName, buttonFont);
    tabButton->setCharacterSize(14);
}

void MainMenu::updateAllTextSizes(const TextSize size)
{
    _currentTextSize = size;

    updateComponentTextSizes(size);
    updateLabelTextSizes(size);
    updateButtonTextSizes(size);
    updateBindingTextSizes(size);
    updateSelectTextSizes(size);

    if (settingsModal) {
        settingsModal->updateTextSizes(size);
    }
}

void MainMenu::updateComponentTextSizes(const TextSize size) const
{
    for (const auto &component : _components) {
        if (auto *const textField
            = dynamic_cast<TextField *>(component.get())) {
            textField->updateTextSize(size);
        }
        if (auto *const button = dynamic_cast<Button *>(component.get())) {
            button->updateTextSize(size);
        }
        if (auto *const textInput
            = dynamic_cast<TextInput *>(component.get())) {
            textInput->updateTextSize(size);
        }
    }
}

void MainMenu::updateLabelTextSizes(const TextSize size)
{
    if (volumeText) {
        volumeText->updateTextSize(size);
    }
    if (masterLabel) {
        masterLabel->updateTextSize(size);
    }
    if (musicLabel) {
        musicLabel->updateTextSize(size);
    }
    if (effectsLabel) {
        effectsLabel->updateTextSize(size);
    }
    if (bindsText) {
        bindsText->updateTextSize(size);
    }
    if (keyboardTitle) {
        keyboardTitle->updateTextSize(size);
    }
    if (controllerTitle) {
        controllerTitle->updateTextSize(size);
    }
    if (moveLabel) {
        moveLabel->updateTextSize(size);
    }
    if (shootLabel) {
        shootLabel->updateTextSize(size);
    }
    if (graphicsText) {
        graphicsText->updateTextSize(size);
    }
    if (colorblindText) {
        colorblindText->updateTextSize(size);
    }
    if (controllerWarning) {
        controllerWarning->updateTextSize(size);
    }
}

void MainMenu::updateButtonTextSizes(const TextSize size)
{
    if (masterMuteButton) {
        masterMuteButton->updateTextSize(size);
    }
    if (musicMuteButton) {
        musicMuteButton->updateTextSize(size);
    }
    if (effectsMuteButton) {
        effectsMuteButton->updateTextSize(size);
    }
}

void MainMenu::updateBindingTextSizes(const TextSize size)
{
    if (shootBinding) {
        shootBinding->updateTextSize(size);
    }
    if (upBinding) {
        upBinding->updateTextSize(size);
    }
    if (downBinding) {
        downBinding->updateTextSize(size);
    }
    if (leftBinding) {
        leftBinding->updateTextSize(size);
    }
    if (rightBinding) {
        rightBinding->updateTextSize(size);
    }
    if (controllerShootBinding) {
        controllerShootBinding->updateTextSize(size);
    }
}

void MainMenu::updateSelectTextSizes(const TextSize size)
{
    if (textSizeSelect) {
        textSizeSelect->updateTextSize(size);
    }
    if (controlTypeSelect) {
        controlTypeSelect->updateTextSize(size);
    }
    if (stickSelect) {
        stickSelect->updateTextSize(size);
    }
}

void MainMenu::handleControllerNavigation(sf::RenderWindow &window)
{
    if (sf::Joystick::isButtonPressed(0, 0)) {
        auto *const button = dynamic_cast<Button *>(
            _components[selectedComponentIndex].get());
        if (button != nullptr) {
            sf::Event event;
            event.type = sf::Event::MouseButtonPressed;
            event.mouseButton.button = sf::Mouse::Left;
            handleButtonEvents(button, window, event);
        }
    }

    if (sf::Joystick::isButtonPressed(0, 1) && settingsModal
        && settingsModal->isVisible()) {
        settingsModal->hide();
    }

    const float yAxis = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
    if (yAxis > 50) {
        selectedComponentIndex
            = (selectedComponentIndex + 1) % _components.size();
    } else if (yAxis < -50) {
        selectedComponentIndex
            = (selectedComponentIndex - 1 + _components.size())
            % _components.size();
    }

    const size_t componentsSize = _components.size();
    for (size_t i = 0; i < componentsSize; ++i) {
        auto *const button = dynamic_cast<Button *>(_components[i].get());
        if (button != nullptr) {
            button->setBackgroundColor(i == selectedComponentIndex
                    ? sf::Color::Yellow
                    : sf::Color::White);
        }
    }
}

void MainMenu::saveSettings()
{
    ofstream settingsFile("settings.txt");
    if (!settingsFile.is_open()) {
        cerr << "Could not open settings file for writing\n";
        return;
    }

    const TextSize newSize = _currentTextSize;
    updateAllTextSizes(newSize);
    settingsModal->setPendingChanges(true);
    settingsModal->initializeSaveButton();
}

void MainMenu::handleValidateButton()
{
    const string username = _playTextInput->getText();
    cout << "Username entered: " << username << "\n";
    playModal->hide();
}

void MainMenu::initializePlayModal()
{
    playModal = make_unique<Modal>(
        sf::Vector2f(400, 200), buttonFont, "Enter your username");
    playModal->setPosition(364, 236);

    const sf::Vector2f modalPos = playModal->getContent().getPosition();
    const sf::Vector2f modalSize = playModal->getContent().getSize();

    const sf::Vector2f inputPos(
        modalPos.x + (modalSize.x - 200) / 2, modalPos.y + 60);

    validateButton = make_unique<Button>(modalPos.x + (modalSize.x - 100) / 2,
        modalPos.y + 120, 100, 40, "Enter", buttonFont);

    validateButton->setOnClick([this]() { handleValidateButton(); });

    playModal->addComponent(_playTextInput.get());
    playModal->addButton(
        *validateButton, [this]() { handleValidateButton(); });
}

void MainMenu::handleButtonEvents(const Button *const button,
    sf::RenderWindow &window, const sf::Event &event)
{
    if (button->isClicked(window, event)) {
        handleButtonClick(button->getText());
    }
}

void MainMenu::handleTextSizeChange(const string_view option)
{
    TextSize newSize = MEDIUM;

    if (option == "Small") {
        newSize = SMALL;
    } else if (option == "Large") {
        newSize = LARGE;
    } else if (option == "Medium or More") {
        newSize = MEDIUM_OR_MORE;
    }

    updateAllTextSizes(newSize);
}
