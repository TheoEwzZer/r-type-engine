/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** PixelGrid
*/

#include "PixelGrid.hpp"
#include "../../EditorEngine/EditorEngine.hpp"

void PixelGrid::setSpriteParts(
    const vector<sf::Sprite> &spriteParts, sf::Vector2u windowSize)
{
    spriteParts_ = spriteParts;
    float spacing = 60.f;
    float startX = static_cast<float>(windowSize.x) - 120.f;
    float startY = 10.f;

    for (size_t i = 0; i < spriteParts_.size(); ++i) {
        spriteParts_[i].setPosition(startX, startY + i * spacing);
    }
}

const vector<sf::Sprite> &PixelGrid::getSpriteParts() const
{
    return spriteParts_;
}

void PixelGrid::handleDragAndDrop(sf::RenderWindow &window, sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos
                = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (const auto &sprite : spriteParts_) {
                if (sprite.getGlobalBounds().contains(mousePos)) {
                    isDraggingSprite_ = true;
                    draggedSprite_ = sprite;
                    dragOffset_ = mousePos - sprite.getPosition();
                    originalPos_ = sprite.getPosition();
                    draggedSprite_.setColor(sf::Color(255, 255, 255, 180));
                    break;
                }
            }
        }
    }

    if (event.type == sf::Event::MouseMoved) {
        if (isDraggingSprite_) {
            sf::Vector2f mousePos
                = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            draggedSprite_.setPosition(mousePos - dragOffset_);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Left && isDraggingSprite_) {
            sf::Vector2f mousePos
                = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (mousePos.x >= _startPos.x && mousePos.y >= _startPos.y
                && mousePos.x < _startPos.x + _gridSize * _pixelSize * _scale
                && mousePos.y
                    < _startPos.y + _gridSize * _pixelSize * _scale) {

                int gridX = static_cast<int>(
                    (mousePos.x - _startPos.x) / (_pixelSize * _scale));
                int gridY = static_cast<int>(
                    (mousePos.y - _startPos.y) / (_pixelSize * _scale));

                if (gridX >= 0 && gridY >= 0 && gridX < _gridSize
                    && gridY < _gridSize) {
                    sf::Color spriteColor = draggedSprite_.getColor();
                    setPixelColor(gridX, gridY, spriteColor);

                    draggedSprite_.setPosition(
                        _startPos.x + gridX * _pixelSize * _scale,
                        _startPos.y + gridY * _pixelSize * _scale);
                    placedSprites_.push_back(draggedSprite_);
                }
            }
            isDraggingSprite_ = false;
            draggedSprite_.setColor(sf::Color::White);
        }
    }
}

void PixelGrid::draw(sf::RenderWindow &window)
{
    for (auto &row : _grid) {
        for (auto &pixel : row) {
            window.draw(pixel);
        }
    }

    for (const auto &sprite : placedSprites_) {
        window.draw(sprite);
    }

    for (const auto &sprite : spriteParts_) {
        window.draw(sprite);
    }

    if (isDraggingSprite_) {
        window.draw(draggedSprite_);

        sf::FloatRect bounds = draggedSprite_.getGlobalBounds();
        sf::RectangleShape outline;
        outline.setPosition(bounds.left, bounds.top);
        outline.setSize({ bounds.width, bounds.height });
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor(sf::Color::White);
        outline.setOutlineThickness(2.f);
        window.draw(outline);
    }
}

void PixelGrid::clear()
{
    for (int y = 0; y < _gridSize; ++y) {
        for (int x = 0; x < _gridSize; ++x) {
            setPixelColor(x, y, sf::Color::White);
        }
    }
}

bool PixelGrid::setGridSize(int gridSize, int pixelSize, sf::Vector2f startPos)
{
    if (gridSize <= 0 || gridSize > 64 || pixelSize <= 0) {
        return false;
    }

    try {
        vector<vector<sf::Color>> oldPixels;
        if (!_pixels.empty()) {
            oldPixels = _pixels;
        }

        _grid.clear();
        _pixels.clear();
        _grid.resize(gridSize);
        _pixels.resize(
            gridSize, vector<sf::Color>(gridSize, sf::Color::White));

        if (!oldPixels.empty()) {
            for (int y = 0;
                 y < min(gridSize, static_cast<int>(oldPixels.size())); ++y) {
                for (int x = 0;
                     x < min(gridSize, static_cast<int>(oldPixels[y].size()));
                     ++x) {
                    _pixels[y][x] = oldPixels[y][x];
                }
            }
        }

        _gridSize = gridSize;
        _pixelSize = pixelSize;
        _startPos = startPos;

        float scaledPixelSize = _pixelSize * _scale;
        for (int y = 0; y < _gridSize; ++y) {
            _grid[y].resize(_gridSize);
            for (int x = 0; x < _gridSize; ++x) {
                auto &pixel = _grid[y][x];
                pixel.setSize({ scaledPixelSize - 1, scaledPixelSize - 1 });
                pixel.setPosition(_startPos.x + x * scaledPixelSize,
                    _startPos.y + y * scaledPixelSize);
                pixel.setFillColor(_pixels[y][x]);
                pixel.setOutlineThickness(1);
                pixel.setOutlineColor(sf::Color(200, 200, 200));
            }
        }
        return true;

    } catch (const exception &e) {
        clear();
        return false;
    }
}

void PixelGrid::setScale(float scale)
{
    _scale = scale;
    float scaledPixelSize = _pixelSize * _scale;

    for (int y = 0; y < _gridSize; ++y) {
        for (int x = 0; x < _gridSize; ++x) {
            auto &pixel = _grid[y][x];
            pixel.setSize({ scaledPixelSize - 1, scaledPixelSize - 1 });
            pixel.setPosition(_startPos.x + x * scaledPixelSize,
                _startPos.y + y * scaledPixelSize);
        }
    }
}

bool PixelGrid::saveToFile(const string &filename)
{
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(_gridSize, _gridSize)) {
        return false;
    }

    renderTexture.clear(sf::Color::Transparent);

    sf::RectangleShape pixel;
    pixel.setSize({ 1.f, 1.f });

    for (int y = 0; y < _gridSize; ++y) {
        for (int x = 0; x < _gridSize; ++x) {
            sf::Color pixelColor = _pixels[y][x];
            if (pixelColor != sf::Color::White) {
                pixel.setPosition(
                    static_cast<float>(x), static_cast<float>(y));
                pixel.setFillColor(pixelColor);
                renderTexture.draw(pixel);
            }
        }
    }
    renderTexture.display();
    return renderTexture.getTexture().copyToImage().saveToFile(filename);
}

sf::Color PixelGrid::getPixelColor(int x, int y) const
{
    if (x < 0 || y < 0 || x >= _gridSize || y >= _gridSize) {
        return sf::Color::Black;
    }
    return _pixels[y][x];
}

void PixelGrid::setPixelColor(int x, int y, sf::Color color)
{
    if (x < 0 || y < 0 || x >= _gridSize || y >= _gridSize) {
        return;
    }
    _pixels[y][x] = color;
    _grid[y][x].setFillColor(color);
}

int PixelGrid::getGridSize() const { return _gridSize; }
