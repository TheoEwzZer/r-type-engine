/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** AssetManager
*/

#pragma once
#include <string>
#include <unordered_map>

using namespace std;

namespace rtype {
enum class GameplayAsset {
    BACKGROUND1,
    BACKGROUND2,
    BOSS1,
    BOSS1_DEATH,
    BOSS1_PROJECTILE,
    BOSS2,
    ENEMY_DEATH,
    ENEMY_PROJECTILE,
    FORCE1,
    FORCE2,
    FORCE2_LEFT,
    FORCE_DETACH_PROJECTILE,
    FORCE_PROJECTILE,
    FORCE_PROJECTILE_LEFT,
    MOB1,
    MOB2,
    OBSTACLE_LARGE,
    OBSTACLE_LARGE2,
    OBSTACLE_MEDIUM,
    OBSTACLE_MEDIUM2,
    OBSTACLE_SMALL,
    OBSTACLE_SMALL2,
    PLAYER,
    PLAYER_CHARGED_PROJECTILE,
    PLAYER_DEATH,
    PLAYER_PROJECTILE,
    PLAYER_PROJECTILE_LEFT,
    SPEED_POWERUP,
};

class AssetManager {
public:
    static unsigned short getSpriteSheetId(const GameplayAsset asset);
    static unsigned short getRectX(const GameplayAsset asset);
    static unsigned short getRectY(const GameplayAsset asset);
    static unsigned short getWidth(const GameplayAsset asset);
    static unsigned short getHeight(const GameplayAsset asset);

    static bool assetExists(GameplayAsset asset)
    {
        return spriteSheetIds.find(asset) != spriteSheetIds.end()
            && rectXs.find(asset) != rectXs.end()
            && rectYs.find(asset) != rectYs.end()
            && widths.find(asset) != widths.end()
            && heights.find(asset) != heights.end();
    }

private:
    static const unordered_map<GameplayAsset, unsigned short> spriteSheetIds;
    static const unordered_map<GameplayAsset, unsigned int> textureIds;
    static const unordered_map<GameplayAsset, unsigned short> rectXs;
    static const unordered_map<GameplayAsset, unsigned short> rectYs;
    static const unordered_map<GameplayAsset, unsigned short> widths;
    static const unordered_map<GameplayAsset, unsigned short> heights;
};
}
