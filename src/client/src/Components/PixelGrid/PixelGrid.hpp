#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;

class PixelGrid {
public:
    PixelGrid() = default;
    bool setGridSize(int gridSize, int pixelSize, sf::Vector2f startPos);
    void draw(sf::RenderWindow &window);
    void clear();
    void setPixel(int x, int y, sf::Color color);
    sf::Color getPixel(int x, int y) const;
    void setScale(float scale);

    sf::Vector2f getPosition() const { return _startPos; }

    bool saveToFile(const string &filename);

    void setPixelColor(int x, int y, sf::Color color);
    sf::Color getPixelColor(int x, int y) const;

    void setSpriteParts(
        const vector<sf::Sprite> &spriteParts, sf::Vector2u windowSize);
    const vector<sf::Sprite> &getSpriteParts() const;
    void handleDragAndDrop(sf::RenderWindow &window, sf::Event event);

    bool isDraggingSprite() const { return isDraggingSprite_; }

    void setFocus(bool focus) { isFocused = focus; }

    bool hasFocus() const { return isFocused; }

    int getGridSize() const;

    const vector<sf::Sprite> &getPlacedSprites() const
    {
        return placedSprites_;
    }

    int getPixelSize() const { return _pixelSize; }

    float getScale() const { return _scale; }

private:
    vector<sf::Sprite> placedSprites_;
    vector<vector<sf::RectangleShape>> _grid;
    vector<vector<sf::Color>> _pixels;
    int _gridSize = 16;
    int _pixelSize = 20;
    sf::Vector2f _startPos { 300.f, 50.f };
    float _scale = 1.0f;

    vector<sf::Sprite> spriteParts_;
    bool isDraggingSprite_ = false;
    sf::Sprite draggedSprite_;
    sf::Vector2f dragOffset_;

    bool isFocused = false;

    sf::Vector2f originalPos_;
};
