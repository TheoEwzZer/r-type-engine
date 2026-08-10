/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** GameEngine
*/

#define _USE_MATH_DEFINES
#include "GameEngine.hpp"
#include <random>
#include <cmath>

using namespace rtype;


void GameEngine::initializeObstacles()
{
    const int minSpacing = 300;
    const int maxX = ::WINDOW_WIDTH;

    int currentX = minSpacing;
    uniform_int_distribution xSpacingDist(minSpacing, minSpacing * 2);
    uniform_int_distribution obstacleTypeDist(0, 2);

    while (currentX < maxX) {
        const auto obstacleEntity
            = make_shared<Entity>(registry.spawnEntity());
        const int x = currentX;
        GameplayAsset obstacleAsset = OBSTACLE_LARGE;
        switch (obstacleTypeDist(gen)) {
            case 0:
                obstacleAsset = OBSTACLE_SMALL;
                break;
            case 1:
                obstacleAsset = OBSTACLE_MEDIUM;
                break;
            default:
                obstacleAsset = OBSTACLE_LARGE;
                break;
        }

        const unsigned short width = AssetManager::getWidth(obstacleAsset);
        const unsigned short height = AssetManager::getHeight(obstacleAsset);
        const unsigned short rectX = AssetManager::getRectX(obstacleAsset);
        const unsigned short rectY = AssetManager::getRectY(obstacleAsset);

        registry.emplaceComponent<Position>(
            *obstacleEntity, x, ::WINDOW_HEIGHT - (height * 2));
        registry.emplaceComponent<Collider>(
            *obstacleEntity, width, height, 2.0f, 2.0f);
        registry.emplaceComponent<Controllable>(
            *obstacleEntity, false, false, true, false, 1 * currentLevel);
        obstacles.emplace_back(AssetManager::getSpriteSheetId(obstacleAsset),
            rectX, rectY, width, height, x, ::WINDOW_HEIGHT - (height * 2), 2,
            2, static_cast<unsigned int>(*obstacleEntity), 0);
        currentObstacles.emplace_back(
            AssetManager::getSpriteSheetId(obstacleAsset), rectX, rectY, width,
            height, x, ::WINDOW_HEIGHT - (height * 2), 2.0f, 2.0f,
            static_cast<unsigned int>(*obstacleEntity), 0);

        currentX += xSpacingDist(gen);
    }
}

void GameEngine::runSystems() { registry.runSystems(); }

void GameEngine::update()
{
    const auto currentTime = steady_clock::now();
    elapsedTime
        = static_cast<float>(
              duration_cast<milliseconds>(currentTime - startTime).count())
        / 1000.0f;
    elapsedTimeLevel += RENDER_INTERVAL;

    if ((elapsedTimeLevel >= 10.0f) && (!forceCreated)) {
        const auto force = make_shared<Entity>(registry.spawnEntity());
        int posX = -AssetManager::getWidth(FORCE1);
        int posY = ::WINDOW_HEIGHT / 2;
        registry.emplaceComponent<Position>(*force, posX, posY);
        registry.emplaceComponent<Animation>(*force, 64, 6, 0, FORCE1);
        registry.emplaceComponent<Force>(*force);
        registry.emplaceComponent<Controllable>(
            *force, false, false, false, true, 2);
        registry.emplaceComponent<Drawable>(*force,
            Sprite { AssetManager::getSpriteSheetId(FORCE1),
                AssetManager::getRectX(FORCE1), AssetManager::getRectY(FORCE1),
                AssetManager::getWidth(FORCE1),
                AssetManager::getHeight(FORCE1), posX, posY, 1.5f, 1.5f,
                static_cast<unsigned int>(*force), 0 });
        forceCreated = true;
    }
    static float lastPowerUpTime = 0.0f;
    if (elapsedTime - lastPowerUpTime >= 15.0f) {
        spawnSpeedPowerUp();
        lastPowerUpTime = elapsedTime;
    }
    if ((!isBossFight) && (!boss1Defeated) && (currentLevel == 1)
        && (elapsedTimeLevel >= boss1SpawnTime)) {
        spawnBoss1();
        isBossFight = true;
        playerEvents.emplace_back(Event::BOSS_FIGHT, 1);
        return;
    }
    if ((!isBossFight) && (currentLevel == 2)
        && (elapsedTimeLevel >= boss2SpawnTime)) {
        spawnBoss2();
        isBossFight = true;
        playerEvents.emplace_back(Event::BOSS_FIGHT, 1);
        return;
    }

    if (boss1Defeated) {
        playerEvents.emplace_back(Event::BOSS_FIGHT, 0);
        isBossFight = false;
        boss1Defeated = false;
        currentLevel = 2;
        playerLevelEvents.emplace_back(currentLevel);
        elapsedTimeLevel = 0.0f;
    }
    if (boss2Defeated) {
        playerEvents.emplace_back(Event::BOSS_FIGHT, 0);
        isBossFight = false;
        boss2Defeated = false;
        playerLevelEvents.emplace_back(3);
        playerEvents.emplace_back(Event::DESTROY, 0);
    }
    if (!isBossFight) {
        const float spawnTime
            = (currentLevel == 1) ? boss1SpawnTime : boss2SpawnTime;
        if ((currentLevel <= 2) && (elapsedTimeLevel < (spawnTime - 15))) {
            generateObstacles();
            generateEnemies();
        }
    }
}

void GameEngine::systemSpeedBoostDrawable(Registry &registry,
    SparseArray<Position> &positions, SparseArray<SpeedBoost> &speedBoosts)
{
    static bool powerUpExists = false;
    powerUpExists = false;

    const size_t posSize = positions.size();
    for (size_t i = 0; i < posSize; ++i) {
        const auto entity = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(entity)) {
            continue;
        }

        auto &pos = positions[i];
        const auto &speedBoost = speedBoosts[i];

        if ((!pos.has_value()) || (!speedBoost.has_value())) {
            continue;
        }
        if (powerUpExists) {
            registry.killEntity(entity);
            continue;
        }

        powerUpExists = true;
        pos->x -= 2;

        if (pos->x < -50) {
            registry.killEntity(entity);
            continue;
        }
    }
}

void GameEngine::spawnSpeedPowerUp()
{
    const auto powerUpEntity = make_shared<Entity>(registry.spawnEntity());

    const int posY = ::WINDOW_HEIGHT / 2;

    registry.emplaceComponent<Position>(*powerUpEntity, ::WINDOW_WIDTH, posY);
    registry.emplaceComponent<SpeedBoost>(*powerUpEntity);
    registry.emplaceComponent<Collider>(*powerUpEntity, 32, 32, 1.0f, 1.0f);
    registry.emplaceComponent<Drawable>(*powerUpEntity,
        Sprite { AssetManager::getSpriteSheetId(SPEED_POWERUP),
            AssetManager::getRectX(SPEED_POWERUP),
            AssetManager::getRectY(SPEED_POWERUP),
            AssetManager::getWidth(SPEED_POWERUP),
            AssetManager::getHeight(SPEED_POWERUP), ::WINDOW_WIDTH, posY, 2.0f,
            2.0f, static_cast<unsigned int>(*powerUpEntity), 0 });
}

void GameEngine::systemSpeedBoostDuration(Registry &registry,
    SparseArray<SpeedBoost> &speedBoosts,
    SparseArray<Controllable> &controllables) const
{
    const size_t posSize = speedBoosts.size();
    for (size_t i = 0; i < posSize; ++i) {
        auto &boost = speedBoosts[i];
        if ((!boost.has_value()) || (!boost->isActive)) {
            continue;
        }

        const auto now = steady_clock::now();
        const auto duration
            = duration_cast<seconds>(now - boost->activationTime).count();

        if (static_cast<float>(duration)
            >= static_cast<float>(boost->duration)) {
            auto &control = controllables[i];
            if (control.has_value()) {
                control->speed /= static_cast<float>(boost->speedMultiplier);
            }
            registry.removeComponent<SpeedBoost>(
                registry.getEntity(static_cast<unsigned int>(i)));
        }
    }
}

void GameEngine::systemSpeedBoostCollision(Registry &registry,
    SparseArray<Position> &positions, SparseArray<SpeedBoost> &speedBoosts,
    SparseArray<Controllable> &controllables)
{
    const size_t posSize = positions.size();
    for (size_t i = 0; i < posSize; ++i) {
        auto &powerUpPos = positions[i];
        auto &speedBoost = speedBoosts[i];

        if ((!powerUpPos.has_value()) || (!speedBoost.has_value())) {
            continue;
        }
        for (unsigned int j = 0; j < Registry::RESERVED_CLIENT_IDS; ++j) {
            const auto playerEntity = registry.getEntity(j);
            if (!registry.isEntityAlive(playerEntity)) {
                continue;
            }

            auto &playerPos = registry.getComponent<Position>(playerEntity);
            auto &control = controllables[j];

            if ((!playerPos.has_value()) || (!control.has_value())) {
                continue;
            }

            if (checkCollisionWithObstacle(powerUpPos.value(),
                    playerPos.value(), AssetManager::getWidth(SPEED_POWERUP),
                    AssetManager::getHeight(SPEED_POWERUP),
                    AssetManager::getWidth(PLAYER),
                    AssetManager::getHeight(PLAYER), 0, 0)) {
                speedBoost->isActive = true;
                speedBoost->activationTime = steady_clock::now();
                control->speed *= speedBoost->speedMultiplier;
                registry.killEntity(
                    registry.getEntity(static_cast<unsigned int>(i)));
                playerEvents.emplace_back(Event::DESTROY, i);
            }
        }
    }
}

void GameEngine::spawnBoss1()
{
    const auto bossEntity = make_shared<Entity>(registry.spawnEntity());

    const auto posX = static_cast<float>(
        ::WINDOW_WIDTH - (AssetManager::getWidth(BOSS1) * 2));
    const auto posY = static_cast<float>(
        ::WINDOW_HEIGHT - (AssetManager::getHeight(BOSS1) * 2) - 24);

    registry.emplaceComponent<Position>(
        *bossEntity, static_cast<int>(posX), static_cast<int>(posY));

    registry.emplaceComponent<Health>(*bossEntity, 1000);
    registry.emplaceComponent<Enemy>(*bossEntity);

    registry.emplaceComponent<Controllable>(
        *bossEntity, false, false, false, false, 0);

    registry.emplaceComponent<Animation>(*bossEntity, 128, 3, 4, BOSS1);

    registry.emplaceComponent<Collider>(*bossEntity,
        AssetManager::getWidth(BOSS1), AssetManager::getHeight(BOSS1), 2.0f,
        2.0f);

    registry.emplaceComponent<Boss1>(*bossEntity);

    registry.emplaceComponent<Drawable>(*bossEntity,
        Sprite { AssetManager::getSpriteSheetId(BOSS1),
            AssetManager::getRectX(BOSS1), AssetManager::getRectY(BOSS1),
            AssetManager::getWidth(BOSS1), AssetManager::getHeight(BOSS1),
            static_cast<int>(posX), static_cast<int>(posY), 2.0f, 2.0f,
            static_cast<unsigned int>(*bossEntity), 0 });
}

void GameEngine::spawnBoss2()
{
    const auto bossEntity = make_shared<Entity>(registry.spawnEntity());
    const auto turret1Entity = make_shared<Entity>(registry.spawnEntity());
    const auto turret2Entity = make_shared<Entity>(registry.spawnEntity());
    const auto turret3Entity = make_shared<Entity>(registry.spawnEntity());

    const float posX
        = (::WINDOW_WIDTH * (3.0f / 4.0f)) - AssetManager::getWidth(BOSS2);
    const float posY
        = (::WINDOW_HEIGHT / 2.0f) - AssetManager::getHeight(BOSS2);

    const auto turret1PosX = posX + 14.0f;
    const auto turret1PosY = posY + 60.0f;

    const auto turret2PosX = posX + 94.0f;
    const auto turret2PosY = posY + 190.0f;

    const auto turret3PosX = posX + 110.0f;
    const auto turret3PosY = posY + 240.0f;

    registry.emplaceComponent<Position>(*turret1Entity,
        static_cast<int>(turret1PosX), static_cast<int>(turret1PosY));

    registry.emplaceComponent<Position>(*turret2Entity,
        static_cast<int>(turret2PosX), static_cast<int>(turret2PosY));

    registry.emplaceComponent<Position>(*turret3Entity,
        static_cast<int>(turret3PosX), static_cast<int>(turret3PosY));

    registry.emplaceComponent<Position>(
        *bossEntity, static_cast<int>(posX), static_cast<int>(posY));

    registry.emplaceComponent<Health>(*bossEntity, 1000);
    registry.emplaceComponent<Enemy>(*bossEntity);

    registry.emplaceComponent<Controllable>(
        *bossEntity, false, false, false, false, 2);

    registry.emplaceComponent<Controllable>(
        *turret1Entity, false, false, false, false, 2);
    registry.emplaceComponent<Controllable>(
        *turret2Entity, false, false, false, false, 2);
    registry.emplaceComponent<Controllable>(
        *turret3Entity, false, false, false, false, 2);

    registry.emplaceComponent<Animation>(*bossEntity, 128, 2, 4, BOSS2);

    registry.emplaceComponent<Collider>(*bossEntity,
        AssetManager::getWidth(BOSS2), AssetManager::getHeight(BOSS2), 2.0f,
        2.0f);

    registry.emplaceComponent<Boss2>(*bossEntity);

    registry.emplaceComponent<Turret>(*turret1Entity);
    registry.emplaceComponent<Turret>(*turret2Entity);
    registry.emplaceComponent<Turret>(*turret3Entity);

    registry.emplaceComponent<Drawable>(*bossEntity,
        Sprite { AssetManager::getSpriteSheetId(BOSS2),
            AssetManager::getRectX(BOSS2), AssetManager::getRectY(BOSS2),
            AssetManager::getWidth(BOSS2), AssetManager::getHeight(BOSS2),
            static_cast<int>(posX), static_cast<int>(posY), 2.0f, 2.0f,
            static_cast<unsigned int>(*bossEntity), 0 });
}

void GameEngine::generateEnemies()
{
    if (isBossFight) {
        return;
    }

    static float enemySpawnTimer = 0.0f;
    static float enemySpawnInterval = 5.0f;
    enemySpawnTimer += PHYSICS_INTERVAL;

    if (enemySpawnTimer >= enemySpawnInterval) {
        enemySpawnTimer = 0.0f;
        uniform_real_distribution<float> intervalDist(5.0f, 10.0f);
        uniform_int_distribution enemyTypeDist(0, 1);
        enemySpawnInterval = intervalDist(gen);
        const int enemyType = enemyTypeDist(gen);
        if (enemyType == 0) {
            generateEnemy1();
        } else {
            generateEnemy2();
        }
    }
}

void GameEngine::generateEnemy1()
{
    const int spacingX = 50;
    uniform_int_distribution yPositionDist(300, ::WINDOW_HEIGHT - 300);
    const int initialY = yPositionDist(gen);

    for (int i = 0; i < 5; ++i) {
        const int initialX = ::WINDOW_WIDTH + (i * spacingX);

        const auto enemyEntity = make_shared<Entity>(registry.spawnEntity());

        registry.emplaceComponent<Position>(*enemyEntity, initialX, initialY);
        registry.emplaceComponent<Collider>(*enemyEntity,
            AssetManager::getWidth(MOB1), AssetManager::getHeight(MOB1), 2.0f,
            2.0f);
        registry.emplaceComponent<Controllable>(
            *enemyEntity, false, false, true, false, 2 * currentLevel);
        registry.emplaceComponent<Mob1>(*enemyEntity);
        registry.emplaceComponent<Health>(*enemyEntity, 20);
        registry.emplaceComponent<Enemy>(*enemyEntity);
        registry.emplaceComponent<Animation>(*enemyEntity, 64, 0, 6, MOB1);
        registry.emplaceComponent<Drawable>(*enemyEntity,
            Sprite { AssetManager::getSpriteSheetId(MOB1),
                AssetManager::getRectX(MOB1), AssetManager::getRectY(MOB1),
                AssetManager::getWidth(MOB1), AssetManager::getHeight(MOB1),
                initialX, initialY, 2.0f, 2.0f,
                static_cast<unsigned int>(*enemyEntity), 0 });
    }
}

void GameEngine::generateEnemy2()
{
    const auto enemyEntity = make_shared<Entity>(registry.spawnEntity());
    const Position playerPos
        = findNearestPlayer(::WINDOW_WIDTH, ::WINDOW_HEIGHT / 2);
    registry.emplaceComponent<Position>(
        *enemyEntity, ::WINDOW_WIDTH, playerPos.y);
    registry.emplaceComponent<Collider>(*enemyEntity,
        AssetManager::getWidth(MOB2), AssetManager::getHeight(MOB2), 2.0f,
        2.0f);
    registry.emplaceComponent<Controllable>(
        *enemyEntity, false, false, true, false, (currentLevel == 1) ? 7 : 11);
    registry.emplaceComponent<Mob2>(*enemyEntity);
    registry.emplaceComponent<Health>(*enemyEntity, 20);
    registry.emplaceComponent<Enemy>(*enemyEntity);
    registry.emplaceComponent<Animation>(*enemyEntity, 64, 7, 6, MOB2);
    registry.emplaceComponent<Drawable>(*enemyEntity,
        Sprite { AssetManager::getSpriteSheetId(MOB2),
            AssetManager::getRectX(MOB2), AssetManager::getRectY(MOB2),
            AssetManager::getWidth(MOB2), AssetManager::getHeight(MOB2),
            ::WINDOW_WIDTH, playerPos.y, 2.0f, 2.0f,
            static_cast<unsigned int>(*enemyEntity), 0 });
}

void GameEngine::generateObstacles()
{
    static float obstacleSpawnTimer = 0.0f;
    static float obstacleSpawnInterval = 2.0f;
    obstacleSpawnTimer += PHYSICS_INTERVAL;

    if (obstacleSpawnTimer >= obstacleSpawnInterval) {
        obstacleSpawnTimer = 0.0f;
        uniform_real_distribution<float> intervalDist(2.0f, 10.0f);
        obstacleSpawnInterval = intervalDist(gen);
        const auto obstacleEntity
            = make_shared<Entity>(registry.spawnEntity());
        GameplayAsset obstacleAsset = OBSTACLE_LARGE;
        uniform_int_distribution obstacleTypeDist(0, 2);
        if (currentLevel == 1) {
            switch (obstacleTypeDist(gen)) {
                case 0:
                    obstacleAsset = OBSTACLE_SMALL;
                    break;
                case 1:
                    obstacleAsset = OBSTACLE_MEDIUM;
                    break;
                default:
                    obstacleAsset = OBSTACLE_LARGE;
                    break;
            }
        } else {
            switch (obstacleTypeDist(gen)) {
                case 0:
                    obstacleAsset = OBSTACLE_SMALL2;
                    break;
                case 1:
                    obstacleAsset = OBSTACLE_MEDIUM2;
                    break;
                default:
                    obstacleAsset = OBSTACLE_LARGE2;
                    break;
            }
        }
        const unsigned short width = AssetManager::getWidth(obstacleAsset);
        const unsigned short height = AssetManager::getHeight(obstacleAsset);
        const int x = ::WINDOW_WIDTH;
        int y = ::WINDOW_HEIGHT - (height * 2);
        if (currentLevel != 1) {
            y -= 24;
        }
        registry.emplaceComponent<Position>(*obstacleEntity, x, y);
        registry.emplaceComponent<Collider>(
            *obstacleEntity, width, height, 2.0f, 2.0f);
        registry.emplaceComponent<Controllable>(
            *obstacleEntity, false, false, true, false, 1 * currentLevel);
        obstacles.emplace_back(AssetManager::getSpriteSheetId(obstacleAsset),
            AssetManager::getRectX(obstacleAsset),
            AssetManager::getRectY(obstacleAsset), width, height, x,
            ::WINDOW_HEIGHT - (height * 2), 2.0f, 2.0f,
            static_cast<unsigned int>(*obstacleEntity), 0);
        currentObstacles.emplace_back(
            AssetManager::getSpriteSheetId(obstacleAsset),
            AssetManager::getRectX(obstacleAsset),
            AssetManager::getRectY(obstacleAsset), width, height, x,
            ::WINDOW_HEIGHT - (height * 2), 2.0f, 2.0f,
            static_cast<unsigned int>(*obstacleEntity), 0);

        if (currentLevel == 1) {
            return;
        }

        const auto ceilingObstacleEntity
            = make_shared<Entity>(registry.spawnEntity());
        registry.emplaceComponent<Position>(*ceilingObstacleEntity, x, 24);
        registry.emplaceComponent<Collider>(
            *ceilingObstacleEntity, width, height, 2.0f, 2.0f);
        registry.emplaceComponent<Controllable>(*ceilingObstacleEntity, false,
            false, true, false, 1 * currentLevel);
        obstacles.emplace_back(AssetManager::getSpriteSheetId(obstacleAsset),
            AssetManager::getRectX(obstacleAsset),
            static_cast<unsigned short>(
                AssetManager::getRectY(obstacleAsset) - height),
            width, height, x, 24, 2.0f, 2.0f,
            static_cast<unsigned int>(*ceilingObstacleEntity), 0);
        currentObstacles.emplace_back(
            AssetManager::getSpriteSheetId(obstacleAsset),
            AssetManager::getRectX(obstacleAsset),
            static_cast<unsigned short>(
                AssetManager::getRectY(obstacleAsset) - height),
            width, height, x, 24, 2.0f, 2.0f,
            static_cast<unsigned int>(*ceilingObstacleEntity), 0);
    }
}

bool GameEngine::checkCollisionWithGround(const Position &pos1) const
{
    const bool isCollidingWithGround = pos1.y > (::WINDOW_HEIGHT - 48);
    const bool isCollidingWithCeiling = (pos1.y < 24) && (currentLevel > 1);

    return (isCollidingWithGround || isCollidingWithCeiling);
}

bool GameEngine::checkCollisionWithObstacle(const Position &pos1,
    const Position &pos2, const int width1, const int height1,
    const int width2, const int height2, const unsigned short rotation1,
    const unsigned short rotation2) const
{
    vector<pair<float, float>> rect1Corners
        = { rotatePoint(static_cast<float>(-width1 / 2.0),
                static_cast<float>(-height1 / 2.0), rotation1),
              rotatePoint(static_cast<float>(width1 / 2.0),
                  static_cast<float>(-height1 / 2.0), rotation1),
              rotatePoint(static_cast<float>(width1 / 2.0),
                  static_cast<float>(height1 / 2.0), rotation1),
              rotatePoint(static_cast<float>(-width1 / 2.0),
                  static_cast<float>(height1 / 2.0), rotation1) };

    vector<pair<float, float>> rect2Corners
        = { rotatePoint(static_cast<float>(-width2 / 2.0),
                static_cast<float>(-height2 / 2.0), rotation2),
              rotatePoint(static_cast<float>(width2 / 2.0),
                  static_cast<float>(-height2 / 2.0), rotation2),
              rotatePoint(static_cast<float>(width2 / 2.0),
                  static_cast<float>(height2 / 2.0), rotation2),
              rotatePoint(static_cast<float>(-width2 / 2.0),
                  static_cast<float>(height2 / 2.0), rotation2) };

    for (auto &[x, y] : rect1Corners) {
        x += static_cast<float>(pos1.x) + (static_cast<float>(width1) / 2.0f);
        y += static_cast<float>(pos1.y) + (static_cast<float>(height1) / 2.0f);
    }
    for (auto &[x, y] : rect2Corners) {
        x += static_cast<float>(pos2.x) + (static_cast<float>(width2) / 2.0f);
        y += static_cast<float>(pos2.y) + (static_cast<float>(height2) / 2.0f);
    }

    const vector<pair<float, float>> axes
        = { { rect1Corners[1].first - rect1Corners[0].first,
                rect1Corners[1].second - rect1Corners[0].second },
              { rect1Corners[1].first - rect1Corners[2].first,
                  rect1Corners[1].second - rect1Corners[2].second },
              { rect2Corners[1].first - rect2Corners[0].first,
                  rect2Corners[1].second - rect2Corners[0].second },
              { rect2Corners[1].first - rect2Corners[2].first,
                  rect2Corners[1].second - rect2Corners[2].second } };

    return ranges::all_of(
        axes, [this, &rect1Corners, &rect2Corners](const auto &axis) {
            return overlapOnAxis(rect1Corners, rect2Corners, axis);
        });
}

pair<float, float> GameEngine::rotatePoint(
    const float x, const float y, const unsigned short angle) const
{
    const auto rad = static_cast<float>(angle * (M_PI / 180.0));
    const float cosA = cos(rad);
    const float sinA = sin(rad);
    return make_pair((x * cosA) - (y * sinA), (x * sinA) + (y * cosA));
}

pair<float, float> GameEngine::project(
    const vector<pair<float, float>> &corners,
    const pair<float, float> &axis) const
{
    float min = ((corners[0].first * axis.first)
        + (corners[0].second * axis.second));
    float max = min;
    for (const auto &[x, y] : corners) {
        const float projection = (x * axis.first) + (y * axis.second);
        if (projection < min) {
            min = projection;
        }
        if (projection > max) {
            max = projection;
        }
    }
    return make_pair(min, max);
}

bool GameEngine::overlapOnAxis(const vector<pair<float, float>> &corners1,
    const vector<pair<float, float>> &corners2,
    const pair<float, float> &axis) const
{
    const auto [min1, max1] = project(corners1, axis);
    const auto [min2, max2] = project(corners2, axis);
    return !((max1 < min2) || (max2 < min1));
}

Position GameEngine::findNearestPlayer(const int x, const int y)
{
    Position nearestPlayerPos { 0, 0 };
    float minDistance = numeric_limits<float>::max();
    auto &players = registry.getComponents<Player>();
    auto &positions = registry.getComponents<Position>();
    for (size_t i = 0; i < players.size(); ++i) {
        if (!players[i].has_value() || !positions[i].has_value()) continue;
        auto &playerPosOpt = positions[i];
        if (playerPosOpt.has_value()) {
            const int dx = playerPosOpt->x - x;
            const int dy = playerPosOpt->y - y;
            const auto distance = static_cast<float>(::sqrt(
                static_cast<double>(dx * dx) + static_cast<double>(dy * dy)));
            if (distance < minDistance) {
                minDistance = distance;
                nearestPlayerPos = *playerPosOpt;
            }
        }
    }
    return nearestPlayerPos;
}

void GameEngine::updateAnimation(
    const bool isDeath, optional<Animation> &animation) const
{
    if ((!animation.has_value()) || (!config.enableAnimation)) {
        return;
    }
    const auto now = steady_clock::now();
    const auto duration
        = duration_cast<milliseconds>(now - animation->lastFrameUpdate)
              .count();
    if (cmp_greater(duration, animation->animationSpeed)) {
        animation->currentFrame += 1;
        if ((!isDeath)
            && (animation->currentFrame > (animation->animationLength - 1))) {
            animation->currentFrame = 0;
        }
        if (isDeath
            && (animation->currentFrame
                > (animation->animationLengthDeath - 1))) {
            animation->currentFrame = 0;
        }
        animation->lastFrameUpdate = now;
    }
}

void GameEngine::updatePlayerDeathSprite(
    Sprite &sprite, optional<Animation> &animation) const
{
    const auto x = static_cast<unsigned short>(
        AssetManager::getRectX(PLAYER_DEATH)
        + (AssetManager::getWidth(PLAYER_DEATH) * animation->currentFrame));

    updateAnimation(true, animation);

    sprite.spritesheetIndex = AssetManager::getSpriteSheetId(PLAYER_DEATH);
    sprite.y = AssetManager::getRectY(PLAYER_DEATH);
    sprite.x = x;
    sprite.width = AssetManager::getWidth(PLAYER_DEATH);
    sprite.height = AssetManager::getHeight(PLAYER_DEATH);
}

void GameEngine::updateEnemyDeathSprite(
    Sprite &sprite, optional<Animation> &animation) const
{
    const auto x
        = static_cast<unsigned short>(AssetManager::getRectX(ENEMY_DEATH)
            + (AssetManager::getWidth(ENEMY_DEATH) * animation->currentFrame));

    updateAnimation(true, animation);

    sprite.spritesheetIndex = AssetManager::getSpriteSheetId(ENEMY_DEATH);
    sprite.y = AssetManager::getRectY(ENEMY_DEATH);
    sprite.x = x;
    sprite.width = AssetManager::getWidth(ENEMY_DEATH);
    sprite.height = AssetManager::getHeight(ENEMY_DEATH);
    sprite.rotation = 0;
}

void GameEngine::updateBoss1DeathSprite(
    Sprite &sprite, optional<Animation> &animation) const
{
    const auto x
        = static_cast<unsigned short>(AssetManager::getRectX(BOSS1_DEATH)
            + (AssetManager::getWidth(BOSS1_DEATH) * animation->currentFrame));

    updateAnimation(true, animation);

    sprite.spritesheetIndex = AssetManager::getSpriteSheetId(BOSS1_DEATH);
    sprite.y = AssetManager::getRectY(BOSS1_DEATH);
    sprite.x = x;
    sprite.width = AssetManager::getWidth(BOSS1_DEATH);
    sprite.height = AssetManager::getHeight(BOSS1_DEATH);
    sprite.rotation = 0;
}


void GameEngine::detachForcesFromPlayer(
    Registry &registry, const ecs::Entity playerEntity) const
{
    const auto forceEntities = registry.getEntities<Force>();
    for (const auto &[forceEntity, force] : forceEntities) {
        if (!registry.isEntityAlive(forceEntity)) {
            continue;
        }
        const auto &forcePosOpt = registry.getComponent<Position>(forceEntity);
        if (!forcePosOpt.has_value()) {
            continue;
        }
        auto &forceComp = registry.getComponent<Force>(forceEntity);
        if (forceComp->playerId == -1) {
            continue;
        }
        if (forceComp->playerId == playerEntity.getIdShort()) {
            auto &playerComp = registry.getComponent<Player>(playerEntity);
            forceComp->playerId = -1;
            playerComp->isForceAttached = false;
            forceComp->attached = false;
        }
    }
}
