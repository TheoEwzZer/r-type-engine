/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** MainMenu.hpp
*/

#pragma once

#include "../Client.hpp"
#include "../ClientEngine/ClientEngine.hpp"
#include "../Components/Button/Button.hpp"
#include "../Components/Modal/Modal.hpp"
#include "../Components/Select/Select.hpp"
#include "../Components/Slider/Slider.hpp"
#include "../Components/Textfield/Textfield.hpp"
#include "../Components/Textinput/Textinput.hpp"
#include "Components/Checkbox/Checkbox.hpp"
#include "Components/KeybindInput/KeybindInput.hpp"
#include "MainMenuException.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <vector>

class EditorEngine;

class MainMenu {
public:
    MainMenu(ClientEngine &engine, Client &client,
        const shared_ptr<EditorEngine> editor);
    ~MainMenu() = default;

    void initialize();
    void render(sf::RenderWindow &window) const;
    void handleEvents(sf::RenderWindow &window);
    void handleButtonEvents(const Button *const button,
        sf::RenderWindow &window, const sf::Event &event);
    void handleSettingsModalEvents(
        const sf::Event &event, const sf::RenderWindow &window);
    void handlePlayModalEvents(const sf::Event &event);
    bool menuLoop(sf::RenderWindow &window);
    void checkControllerConnection();
    void initializeComponents();
    void initializeTabs();
    void updateKeybindDisplays();
    void initializeKeybindings();
    void initializeTextFields();
    void initializeVolumeControls();
    void handleControllerNavigation(sf::RenderWindow &window);
    void saveSettings();
    void handleTextSizeChange(const string_view option);
    void handleValidateButton();
    bool handleGlobalEvents(const sf::Event &event) const;
    void handleComponentEvents(
        const sf::Event &event, sf::RenderWindow &window);
    void handleButtonClick(const string_view buttonText);
    void updateComponentTextSizes(const TextSize size) const;
    void updateBindingTextSizes(const TextSize size);
    void updateSelectTextSizes(const TextSize size);
    void updateLabelTextSizes(const TextSize size);
    void updateButtonTextSizes(const TextSize size);
    void initializePlayModal();
    void toggleController();
    void updateAllTextSizes(const TextSize size);
    void updateControlsFromSettings();

    sf::Vector2f getCursorPosition() const { return cursorPosition; }

    float getCursorSpeed() const { return _cursorSpeed; }

    void handleControllerCursor(sf::RenderWindow &window);

    void setEditorActive(bool active) { editorActive_ = active; }

private:
    bool hasController;
    bool usingController;
    ClientEngine &_clientEngine;
    unique_ptr<Select> controlTypeSelect;
    rtype::Client &_client;
    sf::RenderWindow &_window;
    vector<unique_ptr<IComponent>> _components;
    sf::Font _titleFont;
    sf::Font buttonFont;
    unique_ptr<Modal> settingsModal;
    sf::RectangleShape overlay;
    unique_ptr<Slider> masterVolumeSlider;
    unique_ptr<Slider> _musicVolumeSlider;
    unique_ptr<Slider> effectsVolumeSlider;
    unique_ptr<Button> masterMuteButton;
    unique_ptr<Button> musicMuteButton;
    unique_ptr<Button> effectsMuteButton;
    unique_ptr<KeybindInput> shootBinding;
    unique_ptr<KeybindInput> upBinding;
    unique_ptr<KeybindInput> downBinding;
    unique_ptr<KeybindInput> leftBinding;
    unique_ptr<KeybindInput> rightBinding;
    unique_ptr<Checkbox> invertXAxis;
    unique_ptr<Checkbox> invertYAxis;
    unique_ptr<TextField> volumeText;
    unique_ptr<TextField> masterLabel;
    unique_ptr<TextField> musicLabel;
    unique_ptr<TextField> effectsLabel;
    unique_ptr<TextField> controllerWarning;
    unique_ptr<TextField> bindsText;
    unique_ptr<TextField> keyboardTitle;
    unique_ptr<TextField> controllerTitle;
    unique_ptr<TextField> moveLabel;
    unique_ptr<TextField> shootLabel;
    unique_ptr<TextField> graphicsText;
    unique_ptr<TextField> colorblindText;

    unique_ptr<KeybindInput> controllerShootBinding;
    unique_ptr<KeybindInput> _controllerMoveBinding;
    unique_ptr<Select> stickSelect;
    unique_ptr<Select> textSizeSelect;
    TextSize _currentTextSize;
    size_t selectedComponentIndex;

    unique_ptr<Modal> playModal;
    unique_ptr<TextInput> _playTextInput;
    unique_ptr<Button> validateButton;
    sf::Vector2f cursorPosition;
    float _cursorSpeed = 10.0f;
    bool previousShootButtonState = false;
    shared_ptr<EditorEngine> _editor;
    bool editorActive_ = false;

    unique_ptr<Button> editorButton;
    sf::Texture _backgroundTexture;
    sf::Texture _mobTexture;
    mutable sf::Sprite _backgroundSprite1;
    mutable sf::Sprite _backgroundSprite2;
    mutable vector<sf::Sprite> _floatingSprites;
    void initializeFloatingSprites();
    void updateFloatingSprites() const;
    void drawFloatingSprites(sf::RenderWindow &window) const;
    vector<sf::Texture> _textures;
    sf::Texture _logoTexture;
    sf::Sprite _logoSprite;

    unique_ptr<KeybindInput> controllerBackBinding; // Added declaration
};
