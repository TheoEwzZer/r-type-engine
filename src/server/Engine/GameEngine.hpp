/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** GameEngine
*/

#pragma once

#include "../Engine/AssetManager.hpp"
#include "../Engine/Components/AI.hpp"
#include "../Engine/Components/Animation.hpp"
#include "../Engine/Components/Boss1.hpp"
#include "../Engine/Components/Boss2.hpp"
#include "../Engine/Components/Collider.hpp"
#include "../Engine/Components/Controllable.hpp"
#include "../Engine/Components/Drawable.hpp"
#include "../Engine/Components/Enemy.hpp"
#include "../Engine/Components/Force.hpp"
#include "../Engine/Components/Health.hpp"
#include "../Engine/Components/Mob1.hpp"
#include "../Engine/Components/Mob2.hpp"
#include "../Engine/Components/Player.hpp"
#include "../Engine/Components/Position.hpp"
#include "../Engine/Components/Projectile.hpp"
#include "../Engine/Components/Respawn.hpp"
#include "../Engine/Components/SpeedBoost.hpp"
#include "../Engine/Components/Turret.hpp"
#include "../Engine/EngineConfig.hpp"
#include "../Engine/Network.hpp"
#include "../Engine/Registry.hpp"
#include "Protocol.hpp"
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;
using namespace rtype::ecs;
using enum rtype::GameplayAsset;

static constexpr int WINDOW_WIDTH = 1128;
static constexpr int WINDOW_HEIGHT = 672;

static constexpr float RENDER_INTERVAL = 1.0f / 60.0f;
static constexpr float PHYSICS_INTERVAL = 1.0f / 20.0f;

namespace rtype {
class GameEngine {
public:
    GameEngine(
        Registry &registry, const EngineConfig &config, Network &network);
    void initializeObstacles();
    void runSystems();
    void update();
    void spawnBoss1();
    void spawnBoss2();
    void addToScore(const unsigned int points);
    void spawnSpeedPowerUp();
    void systemSpeedBoostDrawable(Registry &registry,
        SparseArray<Position> &positions, SparseArray<SpeedBoost> &speedBoosts,
        SparseArray<Drawable> &drawables);
    void systemSpeedBoostCollision(Registry &registry,
        SparseArray<Position> &positions, SparseArray<SpeedBoost> &speedBoosts,
        SparseArray<Controllable> &controllables);
    void systemSpeedBoostDuration(Registry &registry,
        SparseArray<SpeedBoost> &speedBoosts,
        SparseArray<Controllable> &controllables) const;

    void systemPositionControllable(Registry &registry,
        SparseArray<Position> &positions, SparseArray<Controllable> &controls);

    void systemPositionDrawableControllable(Registry &registry,
        SparseArray<Position> &positions, SparseArray<Drawable> &drawables,
        const SparseArray<Controllable> &controllables);

    void systemPositionProjectileDrawable(Registry &registry,
        SparseArray<Position> &positions, SparseArray<Projectile> &projectiles,
        SparseArray<Drawable> &drawables);

    void systemPositionProjectile(Registry &registry,
        SparseArray<Position> &positions,
        SparseArray<Projectile> &projectiles);

    void systemTurretPosition(Registry &registry, SparseArray<Turret> &turrets,
        SparseArray<Position> &positions);

    void systemRespawnDrawable(Registry &registry,
        const SparseArray<Respawn> &respawns,
        SparseArray<Drawable> &drawables);

    void systemPlayerEvents(Registry &registry,
        const SparseArray<Player> &players, SparseArray<Force> &forces);

    void updateAIPosition(const Registry &registry, SparseArray<AI> &ais,
        SparseArray<Controllable> &controllables,
        SparseArray<Position> &positions);

    void systemMob1DrawableControllable(const Registry &registry,
        const SparseArray<Mob1> &mobs, SparseArray<Drawable> &drawables,
        SparseArray<Controllable> &controllables) const;

    void systemMob1Position(Registry &registry, SparseArray<Mob1> &mobs,
        SparseArray<Position> &positions);

    void systemBoss1Position(Registry &registry, SparseArray<Boss1> &boss1,
        SparseArray<Position> &positions);

    void updateBoss2Position(const Registry &registry,
        SparseArray<Boss2> &boss, SparseArray<Turret> &turrets,
        SparseArray<Controllable> &controllables,
        SparseArray<Position> &positions);

    void updateAnimations(Registry &registry,
        SparseArray<Animation> &animations,
        SparseArray<Drawable> &drawables) const;

    void systemUpdateForces(
        Registry &registry, SparseArray<Force> &forces) const;

    void detachForcesFromPlayer(
        Registry &registry, const ecs::Entity playerEntity) const;

    [[nodiscard]] map<udp::endpoint, shared_ptr<ecs::Entity>, less<>>
    getClient() const;

    [[nodiscard]] Network &getNetwork() { return network; }

    [[nodiscard]] const EngineConfig &getConfig() const { return config; }

    [[nodiscard]] unsigned char getLevel() const { return currentLevel; }

    [[nodiscard]] vector<Sprite> &getObstacles() { return obstacles; }

    [[nodiscard]] vector<Sprite> &getCurrentObstacles()
    {
        return currentObstacles;
    }

    [[nodiscard]] vector<PlayerEvent> &getPlayerEvents()
    {
        return playerEvents;
    }

    [[nodiscard]] vector<PlayerEventLife> &getPlayerLifeEvents()
    {
        return playerLifeEvents;
    }

    [[nodiscard]] vector<PlayerEventLevel> &getPlayerLevelEvents()
    {
        return playerLevelEvents;
    }

    [[nodiscard]] float getElapsedTime() const
    {
        return duration<float>(steady_clock::now() - startTime).count();
    }

    Position findNearestPlayer(const int x, const int y);

private:
    [[nodiscard]] bool checkCollisionWithObstacle(const Position &pos1,
        const Position &pos2, const int width1, const int height1,
        const int width2, const int height2, const unsigned short rotation1,
        const unsigned short rotation2) const;
    [[nodiscard]] bool checkCollisionWithGround(const Position &pos1) const;
    pair<float, float> rotatePoint(
        const float x, const float y, const unsigned short angle) const;
    pair<float, float> project(const vector<pair<float, float>> &corners,
        const pair<float, float> &axis) const;
    bool overlapOnAxis(const vector<pair<float, float>> &corners1,
        const vector<pair<float, float>> &corners2,
        const pair<float, float> &axis) const;
    void generateObstacles();
    void generateEnemies();
    void generateEnemy1();
    void generateEnemy2();
    void updateAnimation(
        const bool isDeath, optional<Animation> &animation) const;
    void updatePlayerDeathSprite(
        Sprite &sprite, optional<Animation> &animation) const;
    void updateEnemyDeathSprite(
        Sprite &sprite, optional<Animation> &animation) const;
    void updateBoss1DeathSprite(
        Sprite &sprite, optional<Animation> &animation) const;

    Registry &registry;
    const EngineConfig &config;
    Network &network;
    vector<PlayerEvent> playerEvents;
    vector<PlayerEventLife> playerLifeEvents;
    vector<PlayerEventLevel> playerLevelEvents;
    float elapsedTime = 0.0f;
    steady_clock::time_point startTime = steady_clock::now();
    unsigned char currentLevel = 1;
    float elapsedTimeLevel = 0.0f;
    vector<Sprite> obstacles;
    vector<Sprite> currentObstacles;
    bool isBossFight = false;
    bool boss1Defeated = false;
    bool boss2Defeated = false;
    const float boss1SpawnTime = 90.0f;
    const float boss2SpawnTime = 70.0f;
    random_device rd;
    mt19937 gen;
    bool forceCreated = false;
    unsigned int globalScore = 0;
    float renderAccumulator = 0.0f;
    float physicsAccumulator = 0.0f;
    steady_clock::time_point lastFrameTime;
};
}
