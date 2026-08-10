/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** EditorEngine
*/

#include "EditorEngine.hpp"
#include <filesystem>
#include <iostream>
#include <string>

EditorEngine::EditorEngine(sf::RenderWindow &sharedWindow) :
    window(sharedWindow), m_font(), colorPicker({ 50.f, 50.f }), pixelGrid(),
    gridSizeSelect({ 50.f, 200.f }, { 120.f, 40.f }, m_font, "Grid Size"),
    saveTypeSelect({ 50.f, 400.f }, { 120.f, 40.f }, m_font, "Save Type"),
    brushSizeSelect({ 50.f, 500.f }, { 120.f, 40.f }, m_font, "Brush Size"),
    brushSize(1), saveModal({ 400, 200 }, m_font, "Save Sprite"),
    saveInput({ 0, 0 }, { 200, 40 }, m_font),
    backButton(10.f, window.getSize().y - 40.f, 100.f, 30.f, "Back", m_font)
{
    if (!m_font.loadFromFile(
            "src/client/assets/fonts/Montserrat/Montserrat-Thin.ttf")) {
        throw runtime_error("Failed to load font");
    }
    setupSaveModal();
    initialize();
}

void EditorEngine::setupSaveModal()
{
    sf::Vector2u windowSize(1280, 720);
    float modalX = (windowSize.x - 300) / 2.0f;
    float modalY = (windowSize.y - 150) / 2.0f;

    saveModal.setPosition(modalX, modalY);

    sf::Vector2f modalPos = saveModal.getContent().getPosition();
    sf::Vector2f modalSize = saveModal.getContent().getSize();

    sf::Vector2f inputPos(
        modalPos.x + (modalSize.x - 150) / 2, modalPos.y + 40);
    saveInput.setPosition(inputPos);
    saveInput.setMaxLength(20);

    saveModal.addComponent(&saveInput);
}

void EditorEngine::handleSave()
{
    string selectedType = saveTypeSelect.getSelectedOption();
    filesystem::path savePath;

    if (selectedType == "Part") {
        savePath = "src/client/assets/parts";
    } else if (selectedType == "Sprite") {
        savePath = "src/client/assets/customsprites";
    } else {
        cerr << "Invalid save type selected!" << endl;
        return;
    }

    if (!filesystem::exists(savePath)) {
        filesystem::create_directories(savePath);
    }

    string filename = saveInput.getText();
    if (filename.empty()) {
        cerr << "Filename cannot be empty!" << endl;
        return;
    }

    if (currentMode == EditorMode::GRID) {
        string fullFilename = filename + ".png";
        filesystem::path filePath = savePath / fullFilename;

        if (pixelGrid.saveToFile(filePath.string())) {
            cout << "Grid saved as: " << filePath.string() << endl;
        } else {
            cerr << "Failed to save grid!" << endl;
        }
    }

    saveModal.hide();
}

void EditorEngine::centerGrid()
{
    sf::Vector2u windowSize = window.getSize();
    int pixelSize = 30;
    int currentGridSize = 32;

    if (currentGridSize == 16) {
        pixelSize = 40;
    }

    sf::Vector2f centerPos(
        (windowSize.x - currentGridSize * pixelSize * zoom) / 2.f,
        (windowSize.y - currentGridSize * pixelSize * zoom) / 2.f);
    pixelGrid.setGridSize(currentGridSize, pixelSize, centerPos);
    pixelGrid.setScale(zoom);
}

void EditorEngine::handleZoom(float delta, sf::Vector2i mouseScreenPos)
{
    float oldScale = zoom;
    sf::Vector2f oldPos = pixelGrid.getPosition();
    int pixelSize = pixelGrid.getPixelSize();

    sf::Vector2f worldPos(
        (mouseScreenPos.x - oldPos.x) / (oldScale * pixelSize),
        (mouseScreenPos.y - oldPos.y) / (oldScale * pixelSize));

    const float zoomFactor = 1.1f;
    if (delta > 0) {
        zoom *= zoomFactor;
    } else if (delta < 0) {
        zoom /= zoomFactor;
    }

    zoom = clamp(zoom, 0.5f, 3.0f);

    sf::Vector2f newStartPos(
        mouseScreenPos.x - (worldPos.x * zoom * pixelSize),
        mouseScreenPos.y - (worldPos.y * zoom * pixelSize));

    pixelGrid.setGridSize(pixelGrid.getGridSize(), pixelSize, newStartPos);
    pixelGrid.setScale(zoom);
}

void EditorEngine::start() { initialize(); }

void EditorEngine::initialize()
{
    try {
        if (!m_font.loadFromFile(
                "src/client/assets/fonts/Montserrat/Montserrat-Thin.ttf")) {
            throw runtime_error("Failed to load font");
        }

        float leftMargin = 30.f;
        colorPicker = ColorPicker({ leftMargin - 20, 30.f });
        gridSizeSelect = Select(
            { leftMargin + 90, 150.f }, { 100.f, 30.f }, m_font, "Grid Size");

        saveTypeSelect = Select(
            { leftMargin + 90, 300.f }, { 120.f, 40.f }, m_font, "Save Type");

        brushSizeSelect = Select(
            { leftMargin + 90, 450.f }, { 120.f, 40.f }, m_font, "Brush Type");

        saveTypeSelect.addOption("Part");
        saveTypeSelect.addOption("Sprite");
        saveTypeSelect.setSelectedOption("Part");

        brushSizeSelect.addOption("Small");
        brushSizeSelect.addOption("Medium");
        brushSizeSelect.addOption("Large");
        brushSizeSelect.setSelectedOption("Small");

        brushSizeSelect.setCallback([this](const string &option) {
            if (option == "Small") {
                brushSize = 1;
            } else if (option == "Medium") {
                brushSize = 2;
            } else if (option == "Large") {
                brushSize = 3;
            }
        });

        gridSizeSelect.addOption("16x16");
        gridSizeSelect.addOption("32x32");
        gridSizeSelect.addOption("48x48");
        gridSizeSelect.setCallback([this](const string &option) {
            try {
                int newSize = stoi(option.substr(0, option.find('x')));
                int pixelSize = 25;

                if (newSize == 16) {
                    pixelSize = 30;
                } else if (newSize == 48) {
                    pixelSize = 15;
                }

                sf::Vector2u windowSize = window.getSize();
                sf::Vector2f centerPos(
                    (windowSize.x - newSize * pixelSize * zoom) / 2.f,
                    (windowSize.y - newSize * pixelSize * zoom) / 2.f);
                pixelGrid.setGridSize(newSize, pixelSize, centerPos);
                pixelGrid.setScale(zoom);

                captureGridState();
            } catch (const exception &e) {
                cerr << "Error changing grid size: " << e.what() << endl;
            }
        });

        gridSizeSelect.setSelectedOption("32x32");

        zoom = 0.5f;
        centerGrid();

        centerGrid();

        captureGridState();

        loadSpriteParts("src/client/assets/parts");

    } catch (const exception &e) {
        cerr << "Initialization failed: " << e.what() << endl;
        window.close();
    }
}

void EditorEngine::captureGridState()
{
    GridState state;
    for (int y = 0; y < pixelGrid.getGridSize(); ++y) {
        vector<sf::Color> row;
        for (int x = 0; x < pixelGrid.getGridSize(); ++x) {
            row.push_back(pixelGrid.getPixelColor(x, y));
        }
        state.pixelColors.push_back(row);
    }

    if (currentStateIndex_ < static_cast<int>(gridStates_.size()) - 1) {
        gridStates_.erase(
            gridStates_.begin() + currentStateIndex_ + 1, gridStates_.end());
    }

    gridStates_.push_back(state);
    currentStateIndex_++;
}

void EditorEngine::handlePixelPainting(int mouseX, int mouseY)
{
    if (isDragging) {
        return;
    }

    pixelGrid.setFocus(true);

    int gridX = static_cast<int>((mouseX - pixelGrid.getPosition().x)
        / (pixelGrid.getPixelSize() * pixelGrid.getScale()));
    int gridY = static_cast<int>((mouseY - pixelGrid.getPosition().y)
        / (pixelGrid.getPixelSize() * pixelGrid.getScale()));

    if (gridX < 0 || gridY < 0 || gridX >= pixelGrid.getGridSize()
        || gridY >= pixelGrid.getGridSize()) {
        return;
    }

    if (brushSize == 1) {
        pixelGrid.setPixelColor(gridX, gridY, colorPicker.getCurrentColor());
    } else if (brushSize == 2) {
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                int nx = gridX + dx;
                int ny = gridY + dy;
                if (nx >= 0 && ny >= 0 && nx < pixelGrid.getGridSize()
                    && ny < pixelGrid.getGridSize()) {
                    pixelGrid.setPixelColor(
                        nx, ny, colorPicker.getCurrentColor());
                }
            }
        }
    } else if (brushSize == 3) {
        for (int dx = -2; dx <= 2; ++dx) {
            for (int dy = -2; dy <= 2; ++dy) {
                int nx = gridX + dx;
                int ny = gridY + dy;
                if (nx >= 0 && ny >= 0 && nx < pixelGrid.getGridSize()
                    && ny < pixelGrid.getGridSize()) {
                    pixelGrid.setPixelColor(
                        nx, ny, colorPicker.getCurrentColor());
                }
            }
        }
    }

    captureGridState();
}

void EditorEngine::render()
{
    window.clear(sf::Color(70, 70, 70));
    if (currentMode == EditorMode::GRID) {
        pixelGrid.draw(window);
        colorPicker.draw(window);
        gridSizeSelect.draw(window);
        saveTypeSelect.draw(window);
        brushSizeSelect.draw(window);

        for (const auto &sprite : pixelGrid.getPlacedSprites()) {
            window.draw(sprite);
        }
    }

    backButton.draw(window);

    if (saveModal.isVisible()) {
        saveModal.draw(window);
        saveInput.draw(window);
    }
    window.display();
}

void EditorEngine::handleEvents()
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (menuActive_) {
            break;
        }
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (saveModal.isVisible()) {
            if (event.type == sf::Event::KeyPressed
                && event.key.code == sf::Keyboard::Enter) {
                handleSave();
                continue;
            }
            saveModal.handleEvent(event);
            saveInput.handleEvent(event);
            continue;
        }

        if (currentMode == EditorMode::GRID) {
            pixelGrid.handleDragAndDrop(window, event);
            isDragging = pixelGrid.isDraggingSprite();

            if (!pixelGrid.isDraggingSprite()) {
                colorPicker.handleEvent(event, window);
                gridSizeSelect.handleEvent(event, window);
                saveTypeSelect.handleEvent(event, window);
                brushSizeSelect.handleEvent(event, window);

                if (event.type == sf::Event::MouseWheelScrolled) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    handleZoom(event.mouseWheelScroll.delta, mousePos);
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    handlePixelPainting(
                        event.mouseButton.x, event.mouseButton.y);
                    continue;
                } else if (event.type == sf::Event::MouseMoved) {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        handlePixelPainting(
                            event.mouseMove.x, event.mouseMove.y);
                        continue;
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                bool ctrlPressed = event.key.control;
                if (ctrlPressed && event.key.code == sf::Keyboard::Z) {
                    undo();
                    continue;
                }
                if (event.key.code == sf::Keyboard::Delete) {
                    pixelGrid.clear();
                    captureGridState();
                    continue;
                }
                if (ctrlPressed && event.key.code == sf::Keyboard::Y) {
                    redo();
                    continue;
                }

                if (event.key.code == sf::Keyboard::S && event.key.control) {
                    saveModal.show();
                    continue;
                }
            }
        }
        backButton.handleEvent(event);
        if (backButton.isClicked(window, event)) {
            window.close();
        }
    }
}

void EditorEngine::undo()
{
    if (currentStateIndex_ <= 0) {
        return;
    }

    currentStateIndex_--;
    const GridState &state = gridStates_[currentStateIndex_];

    for (int y = 0; y < pixelGrid.getGridSize(); ++y) {
        for (int x = 0; x < pixelGrid.getGridSize(); ++x) {
            pixelGrid.setPixelColor(x, y, state.pixelColors[y][x]);
        }
    }
}

void EditorEngine::redo()
{
    if (currentStateIndex_ >= static_cast<int>(gridStates_.size()) - 1) {
        return;
    }

    currentStateIndex_++;
    const GridState &state = gridStates_[currentStateIndex_];

    for (int y = 0; y < pixelGrid.getGridSize(); ++y) {
        for (int x = 0; x < pixelGrid.getGridSize(); ++x) {
            pixelGrid.setPixelColor(x, y, state.pixelColors[y][x]);
        }
    }
}

void EditorEngine::run()
{
    menuActive_ = false;
    while (window.isOpen()) {
        handleEvents();
        render();
        if (menuActive_) {
            break;
        }
    }
}

void EditorEngine::loadSpriteParts(const string &directory)
{
    if (!filesystem::exists(directory)) {
        cerr << "Sprite parts directory does not exist: " << directory << endl;
        return;
    }

    float currentYOffset = 10.f;
    float spacing = 10.f;

    for (const auto &entry : filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            sf::Texture texture;
            if (texture.loadFromFile(entry.path().string())) {
                spritePartsTextures.push_back(texture);
                sf::Sprite sprite;
                sprite.setTexture(spritePartsTextures.back());

                float xPos = static_cast<float>(window.getSize().x)
                    - spritePartsTextures.back().getSize().x - 10.f;
                float yPos = currentYOffset;
                sprite.setPosition(xPos, yPos);
                spriteParts.push_back(sprite);

                spritePartsNames.push_back(entry.path().stem().string());

                currentYOffset
                    += spritePartsTextures.back().getSize().y + spacing;

                if (currentYOffset
                    > static_cast<float>(window.getSize().y) - 50.f) {
                    cerr << "Reached maximum displayable sprites." << endl;
                    break;
                }
            } else {
                cerr << "Failed to load texture: " << entry.path().string()
                     << endl;
            }
        }
    }

    if (spriteParts.empty()) {
        cerr << "No sprite parts loaded from directory: " << directory << endl;
    }

    if (!spriteParts.empty()) {
        pixelGrid.setSpriteParts(spriteParts, window.getSize());
    }
}
