#pragma once

#include "../Components/Button/Button.hpp"
#include "../Components/ColorPicker/ColorPicker.hpp"
#include "../Components/Modal/Modal.hpp"
#include "../Components/PixelGrid/PixelGrid.hpp"
#include "../Components/Select/Select.hpp"
#include "../Components/Textinput/Textinput.hpp"
#include "../IEngine.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <stack>
#include <variant>
#include <vector>

using namespace std;

struct GridState {
    vector<vector<sf::Color>> pixelColors;
};

struct PixelAction {
    int x;
    int y;
    sf::Color previousColor;
    sf::Color newColor;
};

struct SpriteAction {
    sf::Sprite sprite;
    bool added;
};

using EditorAction = variant<PixelAction, SpriteAction>;

struct AssemblerSprite {
    sf::Sprite sprite;
    float scaleMultiplier;
};

class EditorEngine {
public:
    EditorEngine();
    EditorEngine(sf::RenderWindow &sharedWindow);
    void start();
    void handleSave();
    void setupSaveModal();
    void centerGrid();
    void handleZoom(float delta);
    void handleZoom(float delta, sf::Vector2i mousePos);
    void initialize();
    void handlePixelPainting(int x, int y);
    void render();
    void loadAssemblerSprites();
    void handleEvents();
    void run();

    void setMenuActive(bool active) { menuActive_ = active; }

    bool isMenuActive() const { return menuActive_; }

    bool getIsDragging() const { return isDragging; }

    void undo();
    void redo();

private:
    sf::RenderWindow &window;
    sf::Font m_font;
    ColorPicker colorPicker;
    PixelGrid pixelGrid;
    Select gridSizeSelect;
    Select saveTypeSelect;
    Select brushSizeSelect;
    int brushSize;
    Modal saveModal;
    TextInput saveInput;
    Button backButton;
    vector<sf::Sprite> placedSprites;
    sf::Sprite draggedSprite;
    bool isDragging = false;
    sf::Vector2f dragOffset;
    float zoom = 1.0f;
    enum class EditorMode { GRID };
    EditorMode currentMode = EditorMode::GRID;
    bool menuActive_ = false;

    vector<GridState> gridStates_;
    int currentStateIndex_ = -1;

    void captureGridState();

    vector<sf::Sprite> spriteParts;
    sf::Texture spritePartsTexture;
    vector<sf::Texture> spritePartsTextures;
    vector<string> spritePartsNames;

    sf::Vector2f _startPos = { 300.f, 50.f };

public:
    void loadSpriteParts(const string &directory);
};
