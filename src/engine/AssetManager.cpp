/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** AssetManager
*/

#include "AssetManager.hpp"

namespace rtype {

const unordered_map<GameplayAsset, unsigned short> AssetManager::spriteSheetIds
    = {
          { GameplayAsset::BACKGROUND1, 4 },
          { GameplayAsset::BACKGROUND2, 4 },
          { GameplayAsset::BOSS1, 2 },
          { GameplayAsset::BOSS1_DEATH, 2 },
          { GameplayAsset::BOSS1_PROJECTILE, 2 },
          { GameplayAsset::BOSS2, 3 },
          { GameplayAsset::ENEMY_DEATH, 6 },
          { GameplayAsset::ENEMY_PROJECTILE, 7 },
          { GameplayAsset::FORCE1, 12 },
          { GameplayAsset::FORCE2, 12 },
          { GameplayAsset::FORCE2_LEFT, 12 },
          { GameplayAsset::FORCE_DETACH_PROJECTILE, 12 },
          { GameplayAsset::FORCE_PROJECTILE, 1 },
          { GameplayAsset::FORCE_PROJECTILE_LEFT, 1 },
          { GameplayAsset::MOB1, 8 },
          { GameplayAsset::MOB2, 5 },
          { GameplayAsset::OBSTACLE_LARGE, 4 },
          { GameplayAsset::OBSTACLE_LARGE2, 4 },
          { GameplayAsset::OBSTACLE_MEDIUM, 4 },
          { GameplayAsset::OBSTACLE_MEDIUM2, 4 },
          { GameplayAsset::OBSTACLE_SMALL, 4 },
          { GameplayAsset::OBSTACLE_SMALL2, 4 },
          { GameplayAsset::PLAYER, 9 },
          { GameplayAsset::PLAYER_CHARGED_PROJECTILE, 1 },
          { GameplayAsset::PLAYER_DEATH, 1 },
          { GameplayAsset::PLAYER_PROJECTILE, 1 },
          { GameplayAsset::PLAYER_PROJECTILE_LEFT, 1 },
          { GameplayAsset::SPEED_POWERUP, 13 },
      };

const unordered_map<GameplayAsset, unsigned short> AssetManager::rectXs = {
    { GameplayAsset::BACKGROUND1, 0 },
    { GameplayAsset::BACKGROUND2, 505 },
    { GameplayAsset::BOSS1, 27 },
    { GameplayAsset::BOSS1_DEATH, 27 },
    { GameplayAsset::BOSS1_PROJECTILE, 576 },
    { GameplayAsset::BOSS2, 0 },
    { GameplayAsset::ENEMY_DEATH, 0 },
    { GameplayAsset::ENEMY_PROJECTILE, 136 },
    { GameplayAsset::FORCE1, 159 },
    { GameplayAsset::FORCE2, 120 },
    { GameplayAsset::FORCE2_LEFT, 318 },
    { GameplayAsset::FORCE_DETACH_PROJECTILE, 89 },
    { GameplayAsset::FORCE_PROJECTILE, 169 },
    { GameplayAsset::FORCE_PROJECTILE_LEFT, 267 },
    { GameplayAsset::MOB1, 7 },
    { GameplayAsset::MOB2, 0 },
    { GameplayAsset::OBSTACLE_LARGE, 127 },
    { GameplayAsset::OBSTACLE_LARGE2, 1447 },
    { GameplayAsset::OBSTACLE_MEDIUM, 1303 },
    { GameplayAsset::OBSTACLE_MEDIUM2, 2087 },
    { GameplayAsset::OBSTACLE_SMALL, 391 },
    { GameplayAsset::OBSTACLE_SMALL2, 2419 },
    { GameplayAsset::PLAYER, 67 },
    { GameplayAsset::PLAYER_CHARGED_PROJECTILE, 201 },
    { GameplayAsset::PLAYER_DEATH, 1 },
    { GameplayAsset::PLAYER_PROJECTILE, 249 },
    { GameplayAsset::PLAYER_PROJECTILE_LEFT, 267 },
    { GameplayAsset::SPEED_POWERUP, 0 },
};

const unordered_map<GameplayAsset, unsigned short> AssetManager::rectYs = {
    { GameplayAsset::BACKGROUND1, 0 },
    { GameplayAsset::BACKGROUND2, 0 },
    { GameplayAsset::BOSS1, 1498 },
    { GameplayAsset::BOSS1_DEATH, 1285 },
    { GameplayAsset::BOSS1_PROJECTILE, 2063 },
    { GameplayAsset::BOSS2, 0 },
    { GameplayAsset::ENEMY_DEATH, 0 },
    { GameplayAsset::ENEMY_PROJECTILE, 6 },
    { GameplayAsset::FORCE1, 35 },
    { GameplayAsset::FORCE2, 69 },
    { GameplayAsset::FORCE2_LEFT, 69 },
    { GameplayAsset::FORCE_DETACH_PROJECTILE, 313 },
    { GameplayAsset::FORCE_PROJECTILE, 136 },
    { GameplayAsset::FORCE_PROJECTILE_LEFT, 136 },
    { GameplayAsset::MOB1, 40 },
    { GameplayAsset::MOB2, 0 },
    { GameplayAsset::OBSTACLE_LARGE, 391 },
    { GameplayAsset::OBSTACLE_LARGE2, 439 },
    { GameplayAsset::OBSTACLE_MEDIUM, 407 },
    { GameplayAsset::OBSTACLE_MEDIUM2, 399 },
    { GameplayAsset::OBSTACLE_SMALL, 431 },
    { GameplayAsset::OBSTACLE_SMALL2, 431 },
    { GameplayAsset::PLAYER, 3 },
    { GameplayAsset::PLAYER_CHARGED_PROJECTILE, 121 },
    { GameplayAsset::PLAYER_DEATH, 343 },
    { GameplayAsset::PLAYER_PROJECTILE, 90 },
    { GameplayAsset::PLAYER_PROJECTILE_LEFT, 90 },
    { GameplayAsset::SPEED_POWERUP, 0 },
};

const unordered_map<GameplayAsset, unsigned short> AssetManager::widths = {
    { GameplayAsset::BACKGROUND1, 376 },
    { GameplayAsset::BACKGROUND2, 534 },
    { GameplayAsset::BOSS1, 155 },
    { GameplayAsset::BOSS1_DEATH, 155 },
    { GameplayAsset::BOSS1_PROJECTILE, 22 },
    { GameplayAsset::BOSS2, 176 },
    { GameplayAsset::ENEMY_DEATH, 32 },
    { GameplayAsset::ENEMY_PROJECTILE, 7 },
    { GameplayAsset::FORCE1, 20 },
    { GameplayAsset::FORCE2, 27 },
    { GameplayAsset::FORCE2_LEFT, 27 },
    { GameplayAsset::FORCE_DETACH_PROJECTILE, 32 },
    { GameplayAsset::FORCE_PROJECTILE, 48 },
    { GameplayAsset::FORCE_PROJECTILE_LEFT, 48 },
    { GameplayAsset::MOB1, 22 },
    { GameplayAsset::MOB2, 21 },
    { GameplayAsset::OBSTACLE_LARGE, 72 },
    { GameplayAsset::OBSTACLE_LARGE2, 136 },
    { GameplayAsset::OBSTACLE_MEDIUM, 48 },
    { GameplayAsset::OBSTACLE_MEDIUM2, 64 },
    { GameplayAsset::OBSTACLE_SMALL, 48 },
    { GameplayAsset::OBSTACLE_SMALL2, 32 },
    { GameplayAsset::PLAYER, 32 },
    { GameplayAsset::PLAYER_CHARGED_PROJECTILE, 32 },
    { GameplayAsset::PLAYER_DEATH, 32 },
    { GameplayAsset::PLAYER_PROJECTILE, 16 },
    { GameplayAsset::PLAYER_PROJECTILE_LEFT, 16 },
    { GameplayAsset::SPEED_POWERUP, 32 },
};

const unordered_map<GameplayAsset, unsigned short> AssetManager::heights = {
    { GameplayAsset::BACKGROUND1, 224 },
    { GameplayAsset::BACKGROUND2, 224 },
    { GameplayAsset::BOSS1, 204 },
    { GameplayAsset::BOSS1_DEATH, 204 },
    { GameplayAsset::BOSS1_PROJECTILE, 20 },
    { GameplayAsset::BOSS2, 144 },
    { GameplayAsset::ENEMY_DEATH, 32 },
    { GameplayAsset::ENEMY_PROJECTILE, 6 },
    { GameplayAsset::FORCE1, 16 },
    { GameplayAsset::FORCE2, 22 },
    { GameplayAsset::FORCE2_LEFT, 22 },
    { GameplayAsset::FORCE_DETACH_PROJECTILE, 4 },
    { GameplayAsset::FORCE_PROJECTILE, 14 },
    { GameplayAsset::FORCE_PROJECTILE_LEFT, 14 },
    { GameplayAsset::MOB1, 22 },
    { GameplayAsset::MOB2, 24 },
    { GameplayAsset::OBSTACLE_LARGE, 80 },
    { GameplayAsset::OBSTACLE_LARGE2, 32 },
    { GameplayAsset::OBSTACLE_MEDIUM, 64 },
    { GameplayAsset::OBSTACLE_MEDIUM2, 64 },
    { GameplayAsset::OBSTACLE_SMALL, 40 },
    { GameplayAsset::OBSTACLE_SMALL2, 32 },
    { GameplayAsset::PLAYER, 12 },
    { GameplayAsset::PLAYER_CHARGED_PROJECTILE, 12 },
    { GameplayAsset::PLAYER_DEATH, 28 },
    { GameplayAsset::PLAYER_PROJECTILE, 4 },
    { GameplayAsset::PLAYER_PROJECTILE_LEFT, 4 },
    { GameplayAsset::SPEED_POWERUP, 32 },
};

unsigned short AssetManager::getSpriteSheetId(const GameplayAsset asset)
{
    return spriteSheetIds.at(asset);
}

unsigned short AssetManager::getRectX(const GameplayAsset asset)
{
    return rectXs.at(asset);
}

unsigned short AssetManager::getRectY(const GameplayAsset asset)
{
    return rectYs.at(asset);
}

unsigned short AssetManager::getWidth(const GameplayAsset asset)
{
    return widths.at(asset);
}

unsigned short AssetManager::getHeight(const GameplayAsset asset)
{
    return heights.at(asset);
}
}
