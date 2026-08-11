/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** GameEngineInit
*/

#define _USE_MATH_DEFINES
#include "GameEngine.hpp"
#include <random>

using namespace rtype;

GameEngine::GameEngine(Registry &registry, const EngineConfig &config) :
    registry(registry), config(config)
{
    gen = mt19937(rd());
    registry.registerComponent<Position>();
    registry.registerComponent<Projectile>();
    registry.registerComponent<Controllable>();
    registry.registerComponent<Health>();
    registry.registerComponent<Collider>();
    registry.registerComponent<Respawn>();
    registry.registerComponent<Drawable>();
    registry.registerComponent<Force>();
    registry.registerComponent<AI>();
    registry.registerComponent<Player>();
    registry.registerComponent<Mob1>();
    registry.registerComponent<Mob2>();
    registry.registerComponent<Turret>();
    registry.registerComponent<Boss1>();
    registry.registerComponent<Boss2>();
    registry.registerComponent<Enemy>();
    registry.registerComponent<Animation>();
    registry.registerComponent<SpeedBoost>();

    registry.addSystem<Position, Controllable>(
        bind_front(&GameEngine::systemPositionControllable, this));

    registry.addSystem<Position, Drawable, Controllable>(
        bind_front(&GameEngine::systemPositionDrawableControllable, this));

    registry.addSystem<Position, Projectile, Drawable>(
        bind_front(&GameEngine::systemPositionProjectileDrawable, this));

    registry.addSystem<Position, Projectile>(
        bind_front(&GameEngine::systemPositionProjectile, this));

    registry.addSystem<Turret, Position>(
        bind_front(&GameEngine::systemTurretPosition, this));

    registry.addSystem<Respawn, Drawable>(
        bind_front(&GameEngine::systemRespawnDrawable, this));

    registry.addSystem<Player, Force>(
        bind_front(&GameEngine::systemPlayerEvents, this));

    if (config.enableObstacles) {
        initializeObstacles();
    }

    registry.addSystem<Mob1, Position>(
        bind_front(&GameEngine::systemMob1Position, this));

    registry.addSystem<Mob1, Drawable, Controllable>(
        bind_front(&GameEngine::systemMob1DrawableControllable, this));

    registry.addSystem<Boss1, Position>(
        bind_front(&GameEngine::systemBoss1Position, this));
    registry.addSystem<Boss1, Position>(
        bind_front(&GameEngine::systemBossMovement, this));

    registry.addSystem<Boss2, Turret, Controllable, Position>(
        bind_front(&GameEngine::updateBoss2Position, this));

    registry.addSystem<AI, Controllable, Position>(
        bind_front(&GameEngine::updateAIPosition, this));

    registry.addSystem<Animation, Drawable>(
        bind_front(&GameEngine::updateAnimations, this));

    registry.addSystem<Force>(
        bind_front(&GameEngine::systemUpdateForces, this));
    registry.addSystem<Position, SpeedBoost>(
        bind_front(&GameEngine::systemSpeedBoostDrawable, this));

    registry.addSystem<Position, SpeedBoost, Controllable>(
        bind_front(&GameEngine::systemSpeedBoostCollision, this));
}

void GameEngine::systemUpdateForces(
    Registry &registry, SparseArray<Force> &forces) const
{
    const size_t minSize = forces.size();
    const auto now = steady_clock::now();
    const auto stopCondition
        = duration_cast<seconds>(now.time_since_epoch()).count();

    for (size_t i = 0; i < minSize; ++i) {
        const auto entity = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(entity)) {
            continue;
        }
        auto &forceOpt = forces[i];
        if (!forceOpt.has_value()) {
            continue;
        }
        auto &force = forceOpt.value();
        if (force.playerId != -1) {
            const auto duration = stopCondition
                - duration_cast<seconds>(force.getTime.time_since_epoch())
                      .count();
            if (duration >= 10) {
                force.level = 2;
                force.power *= 2;

                auto &drawableOpt = registry.getComponents<Drawable>()[i];
                auto &animationOpt = registry.getComponents<Animation>()[i];
                if (animationOpt.has_value()) {
                    auto &animation = animationOpt.value();
                    animation.asset
                        = ((force.level == 2) && force.isForceInFront)
                            || (!force.attached)
                        ? FORCE2
                        : FORCE2_LEFT;
                    animation.currentFrame = 0;
                    animation.lastFrameUpdate = now;
                }
                if (drawableOpt.has_value()) {
                    auto &drawable = drawableOpt.value();
                    if (((force.level == 2) && force.isForceInFront)
                        || (!force.attached)) {
                        drawable.sprite.spritesheetIndex
                            = AssetManager::getSpriteSheetId(FORCE2);
                        drawable.sprite.width = AssetManager::getWidth(FORCE2);
                        drawable.sprite.height
                            = AssetManager::getHeight(FORCE2);
                        drawable.sprite.x = AssetManager::getRectX(FORCE2);
                        drawable.sprite.y = AssetManager::getRectY(FORCE2);
                        drawable.sprite.rotation = 0;
                    } else {
                        drawable.sprite.spritesheetIndex
                            = AssetManager::getSpriteSheetId(FORCE2_LEFT);
                        drawable.sprite.width
                            = AssetManager::getWidth(FORCE2_LEFT);
                        drawable.sprite.height
                            = AssetManager::getHeight(FORCE2_LEFT);
                        drawable.sprite.x
                            = AssetManager::getRectX(FORCE2_LEFT);
                        drawable.sprite.y
                            = AssetManager::getRectY(FORCE2_LEFT);
                        drawable.sprite.rotation = 180;
                    }
                }

                force.getTime = now;
            }
        }
    }
}

void GameEngine::updateAnimations(Registry &registry,
    SparseArray<Animation> &animations, SparseArray<Drawable> &drawables) const
{
    const size_t minSize = min(animations.size(), drawables.size());
    for (size_t i = 0; i < minSize; ++i) {
        const auto &anim = animations[i];
        auto &draw = drawables[i];

        if ((!anim.has_value()) || (!draw.has_value())) {
            continue;
        }
        const auto entity = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(entity)) {
            continue;
        }

        auto &animation = registry.getComponent<Animation>(entity);
        if (!animation.has_value()) {
            continue;
        }
        draw->sprite.x = static_cast<unsigned short>(
            AssetManager::getRectX(animation->asset)
            + (AssetManager::getWidth(animation->asset)
                * animation->currentFrame));

        updateAnimation(false, animation);
    }
}

void GameEngine::updateAIPosition(const Registry & /*unused*/,
    SparseArray<AI> &ais, SparseArray<Controllable> &controllables,
    SparseArray<Position> &positions)
{
    static uniform_int_distribution disX(0, ::WINDOW_WIDTH / 2);
    static uniform_int_distribution disY(160, ::WINDOW_HEIGHT - 200);
    static uniform_int_distribution disDirection(0, 8);
    static auto lastDirectionChangeTime = steady_clock::now();
    static const auto directionChangeInterval = seconds(2);
    const float currentTime = getElapsedTime();
    const size_t minSize
        = min({ ais.size(), controllables.size(), positions.size() });
    for (size_t i = 0; i < minSize; ++i) {
        auto &ai = ais[i];
        auto &control = controllables[i];
        auto &pos = positions[i];
        if ((!ai.has_value()) || (!control.has_value())
            || (!pos.has_value())) {
            continue;
        }
        const auto now = steady_clock::now();
        if ((now - lastDirectionChangeTime) >= directionChangeInterval) {
            control->left = false;
            control->right = false;
            control->up = false;
            control->down = false;
            lastDirectionChangeTime = now;
            const int direction = disDirection(gen);
            if (direction <= 7) {
                if (disX(gen) > pos->x) {
                    control->right = true;
                } else {
                    control->left = true;
                }
                if (direction >= 4) {
                    if (disY(gen) > pos->y) {
                        control->down = true;
                    } else {
                        control->up = true;
                    }
                }
            }
        }
        if (pos->x <= 0) {
            control->left = false;
            control->right = true;
        } else if ((pos->x) >= (::WINDOW_WIDTH / 2)) {
            control->right = false;
            control->left = true;
        }
        if (pos->y <= 160) {
            control->up = false;
            control->down = true;
        } else if ((pos->y) >= (::WINDOW_HEIGHT - 200)) {
            control->down = false;
            control->up = true;
        }
        if ((currentTime - ai->lastShootTime) < ai->shootCooldown) {
            continue;
        }
        const auto projectileEntity
            = make_shared<Entity>(registry.spawnEntity());
        if (!registry.isEntityAlive(*projectileEntity)) {
            return;
        }
        registry.emplaceComponent<Position>(*projectileEntity,
            (pos->x + (AssetManager::getWidth(PLAYER))),
            ((pos->y + ((AssetManager::getHeight(PLAYER)) / 2))
                - ((AssetManager::getHeight(PLAYER_PROJECTILE)) / 2)));
        registry.emplaceComponent<Projectile>(*projectileEntity, 10, false,
            1.0f, 0.0f, ProjectileOrigin::PLAYER);
        registry.emplaceComponent<Drawable>(*projectileEntity,
            Sprite { AssetManager::getSpriteSheetId(PLAYER_PROJECTILE),
                AssetManager::getRectX(PLAYER_PROJECTILE),
                AssetManager::getRectY(PLAYER_PROJECTILE),
                AssetManager::getWidth(PLAYER_PROJECTILE),
                AssetManager::getHeight(PLAYER_PROJECTILE),
                (pos->x + (AssetManager::getWidth(PLAYER))),
                ((pos->y + ((AssetManager::getHeight(PLAYER)) / 2))
                    - ((AssetManager::getHeight(PLAYER_PROJECTILE)) / 2)),
                2, 2, static_cast<unsigned int>(*projectileEntity), 0 });
        ai->lastShootTime = currentTime;
    }
}

void GameEngine::updateBoss2Position(const Registry & /*unused*/,
    SparseArray<Boss2> &boss, SparseArray<Turret> &turrets,
    SparseArray<Controllable> &controllables, SparseArray<Position> &positions)
{
    static uniform_int_distribution disX(
        0, ::WINDOW_WIDTH - AssetManager::getWidth(BOSS2));
    static uniform_int_distribution disY(
        0, ::WINDOW_HEIGHT - AssetManager::getHeight(BOSS2));
    static uniform_int_distribution disDirection(0, 1);
    static auto lastDirectionChangeTime = steady_clock::now();
    static const auto directionChangeInterval = seconds(2);

    const size_t minSize
        = min({ boss.size(), controllables.size(), positions.size() });
    for (size_t i = 0; i < minSize; ++i) {
        const auto &bossEntity = boss[i];
        auto &control = controllables[i];
        auto &pos = positions[i];

        if ((!bossEntity.has_value()) || (!control.has_value())
            || (!pos.has_value())) {
            continue;
        }
        const auto now = steady_clock::now();
        if ((now - lastDirectionChangeTime) >= directionChangeInterval) {
            control->left = false;
            control->right = false;
            control->up = false;
            control->down = false;
            lastDirectionChangeTime = now;
            if (disDirection(gen) == 0) {
                if (disX(gen) > pos->x) {
                    control->right = true;
                } else {
                    control->left = true;
                }
            } else {
                if (disY(gen) > pos->y) {
                    control->down = true;
                } else {
                    control->up = true;
                }
            }
        }
        if (pos->x <= 0) {
            control->left = false;
            control->right = true;
        } else if ((pos->x)
            >= (::WINDOW_WIDTH - (AssetManager::getWidth(BOSS2) * 2))) {
            control->right = false;
            control->left = true;
        }
        if (pos->y <= 24) {
            control->up = false;
            control->down = true;
        } else if ((pos->y) >= (::WINDOW_HEIGHT
                       - ((AssetManager::getHeight(BOSS2) * 2) + 24))) {
            control->down = false;
            control->up = true;
        }
        const auto numTurrets = turrets.size();
        for (unsigned int j = 0; j < numTurrets; ++j) {
            auto &turretControl = controllables[j];
            if ((!turrets[j].has_value()) || (!turretControl.has_value())) {
                continue;
            }
            turretControl->left = control->left;
            turretControl->right = control->right;
            turretControl->up = control->up;
            turretControl->down = control->down;
        }
    }
}

void GameEngine::systemPositionControllable(Registry &registry,
    SparseArray<Position> &positions, SparseArray<Controllable> &controls)
{
    const size_t sizePos = positions.size();
    vector<pair<Entity, Collider *>> colliderCache;
    vector<pair<Entity, Projectile *>> projectileCache;
    colliderCache.reserve(sizePos);
    projectileCache.reserve(sizePos);
    for (size_t i = 0; i < sizePos; ++i) {
        const auto entity = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(entity)) {
            continue;
        }
        auto &collider = registry.getComponent<Collider>(entity);
        if (collider.has_value()) {
            colliderCache.emplace_back(entity, &collider.value());
        }
        auto &projectile = registry.getComponent<Projectile>(entity);
        if (projectile.has_value()) {
            projectileCache.emplace_back(entity, &projectile.value());
        }
    }
    for (size_t i = 0; i < sizePos; ++i) {
        auto &pos = positions[i];
        auto &cont = controls[i];
        if ((!pos.has_value()) || (!cont.has_value())) {
            continue;
        }
        const int newX = pos->x + (cont->left * -cont->speed)
            + (cont->right * cont->speed);
        const int newY
            = pos->y + (cont->up * -cont->speed) + (cont->down * cont->speed);
        if (i < Registry::RESERVED_CLIENT_IDS) {
            pos->x = clamp(newX, 0, ::WINDOW_WIDTH - 48);
            pos->y = clamp(newY, 0, ::WINDOW_HEIGHT - 21);
            cont->up = false;
            cont->down = false;
            cont->left = false;
            cont->right = false;
        } else {
            const auto entity
                = registry.getEntity(static_cast<unsigned int>(i));
            if (isBossFight
                && registry.getComponent<Collider>(entity).has_value()
                && (!registry.getComponent<Enemy>(entity).has_value())) {
                continue;
            }
            pos->x = newX;
            pos->y = newY;
        }
        const auto entityId = static_cast<unsigned int>(i);
        const auto colliderIt
            = ranges::find_if(colliderCache, [entityId](const auto &pair) {
                  return static_cast<unsigned int>(pair.first) == entityId;
              });
        if (colliderIt != colliderCache.end()) {
            const Collider *const collider = colliderIt->second;
            if (static_cast<float>(pos->x)
                < -(static_cast<float>(collider->width) * collider->scaleX
                    * 2.0f)) {
                playerEvents.emplace_back(Event::DESTROY, entityId);
                registry.killEntity(entityId);
                continue;
            }
        }
        const auto projectileIt
            = ranges::find_if(projectileCache, [entityId](const auto &pair) {
                  return static_cast<unsigned int>(pair.first) == entityId;
              });
        if ((projectileIt != projectileCache.end())
            && (colliderIt != colliderCache.end())) {
            const Collider *const collider = colliderIt->second;
            if ((pos->x > ::WINDOW_WIDTH) || (pos->x < collider->width)
                || (pos->y > ::WINDOW_HEIGHT) || (pos->y < collider->height)) {
                playerEvents.emplace_back(Event::DESTROY, entityId);
                registry.killEntity(entityId);
            }
        }

        const auto forceEntities = registry.getEntities<Force>();
        for (const auto &[forceEntity, force] : forceEntities) {
            if (!registry.isEntityAlive(forceEntity)) {
                continue;
            }
            auto &forcePosOpt = registry.getComponent<Position>(forceEntity);
            if (!forcePosOpt.has_value()) {
                continue;
            }
            auto &forceComp = registry.getComponent<Force>(forceEntity);
            if (forceComp->playerId == -1) {
                continue;
            }
            if (forceComp->playerId == static_cast<int>(i)) {
                if (forceComp->attached) {
                    if (forceComp->isForceInFront) {
                        forcePosOpt->x
                            = static_cast<int>(static_cast<float>(pos->x)
                                + (static_cast<float>(
                                       AssetManager::getWidth(PLAYER))
                                    * 1.5f)
                                - 3.0f);
                    } else {
                        const GameplayAsset forceAsset
                            = forceComp->level == 1 ? FORCE1 : FORCE2;
                        forcePosOpt->x
                            = static_cast<int>(static_cast<float>(pos->x)
                                - (static_cast<float>(
                                       AssetManager::getWidth(forceAsset))
                                    * 1.5f)
                                + 3.0f);
                    }
                    if (forceComp->level == 1) {
                        forcePosOpt->y = pos->y;
                    } else {
                        forcePosOpt->y = pos->y - 5;
                    }
                } else {
                    if (forceComp->level == 1) {
                        forcePosOpt->x = pos->x
                            + (static_cast<float>(
                                   AssetManager::getWidth(PLAYER))
                                * 1.5f / 2.0f)
                            - (static_cast<float>(
                                   AssetManager::getWidth(FORCE1))
                                * 1.5f / 2.0f);
                        forcePosOpt->y = pos->y
                            - static_cast<float>(
                                  AssetManager::getHeight(FORCE1))
                                * 2.0f;
                    } else {
                        forcePosOpt->x = pos->x
                            + (static_cast<float>(
                                   AssetManager::getWidth(PLAYER))
                                * 1.5f / 2.0f)
                            - (static_cast<float>(
                                   AssetManager::getWidth(FORCE2))
                                * 1.5f / 2.0f);
                        forcePosOpt->y = pos->y
                            - static_cast<float>(
                                  AssetManager::getHeight(FORCE2))
                                * 2.0f;
                    }
                }

                if (forceComp->attached) {
                    continue;
                }
                const float currentTime = getElapsedTime();
                if ((currentTime - forceComp->lastShootTime)
                    < forceComp->shootCooldown) {
                    continue;
                }
                const auto projectileEntity
                    = make_shared<Entity>(registry.spawnEntity());
                int projectileY = 0;
                if (forceComp->level == 1) {
                    projectileY = forcePosOpt->y
                        + (AssetManager::getHeight(FORCE1) / 2)
                        - (AssetManager::getHeight(FORCE_DETACH_PROJECTILE)
                            / 2);
                } else {
                    projectileY = forcePosOpt->y
                        + (AssetManager::getHeight(FORCE2) / 2)
                        - (AssetManager::getHeight(FORCE_DETACH_PROJECTILE)
                            / 2);
                }

                registry.emplaceComponent<Position>(
                    *projectileEntity, forcePosOpt->x, projectileY);
                registry.emplaceComponent<Projectile>(*projectileEntity, 10,
                    false, 1.0f, 0.0f, ProjectileOrigin::PLAYER);
                registry.emplaceComponent<Animation>(
                    *projectileEntity, 64, 2, 0, FORCE_DETACH_PROJECTILE);
                registry.emplaceComponent<Drawable>(*projectileEntity,
                    Sprite { AssetManager::getSpriteSheetId(
                                 FORCE_DETACH_PROJECTILE),
                        AssetManager::getRectX(FORCE_DETACH_PROJECTILE),
                        AssetManager::getRectY(FORCE_DETACH_PROJECTILE),
                        AssetManager::getWidth(FORCE_DETACH_PROJECTILE),
                        AssetManager::getHeight(FORCE_DETACH_PROJECTILE),
                        forcePosOpt->x, projectileY, 2, 2,
                        static_cast<unsigned int>(*projectileEntity), 0 });
                forceComp->lastShootTime = currentTime;
            }
        }
    }
}

void GameEngine::systemPositionDrawableControllable(Registry &registry,
    SparseArray<Position> &positions, SparseArray<Drawable> &drawables,
    const SparseArray<Controllable> &controllables)
{
    const size_t sizePos = positions.size();
    const size_t sizeDraw = drawables.size();
    const size_t expectedSize = min(sizePos, sizeDraw);
    vector<Sprite> sprites;
    sprites.reserve(expectedSize);
    for (size_t i = 0; i < expectedSize; ++i) {
        const auto entity = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(entity)) {
            continue;
        }
        auto &pos = positions[i];
        auto &draw = drawables[i];
        const auto &cont = controllables[i];

        if (!draw.has_value()) {
            continue;
        }
        if (pos.has_value()) {
            draw->sprite.gameX = pos->x;
            draw->sprite.gameY = pos->y;
        }
        if (i < Registry::RESERVED_CLIENT_IDS) {
            if (cont.has_value()) {
                unsigned short xCoordinate = 67;
                if (cont->prevDown) {
                    xCoordinate = 1;
                } else if (cont->prevUp) {
                    xCoordinate = 133;
                }
                draw->sprite.x = xCoordinate;
            } else {
                auto &animation = registry.getComponent<Animation>(entity);
                if (animation.has_value()) {
                    updatePlayerDeathSprite(draw->sprite, animation);
                }
            }
        } else if (const auto &enemy = registry.getComponent<Enemy>(entity);
                   enemy.has_value() && (!cont.has_value())) {
            auto &animation = registry.getComponent<Animation>(entity);
            if (animation.has_value()) {
                if (registry.getComponent<Boss1>(entity).has_value()) {
                    updateBoss1DeathSprite(draw->sprite, animation);
                } else {
                    updateEnemyDeathSprite(draw->sprite, animation);
                }
            }
        }

        sprites.emplace_back(draw->sprite);
    }
    currentSprites = sprites;
}

void GameEngine::systemPositionProjectileDrawable(Registry &registry,
    SparseArray<Position> &positions, SparseArray<Projectile> &projectiles,
    SparseArray<Drawable> &drawables)
{
    const size_t minSize
        = min({ positions.size(), projectiles.size(), drawables.size() });
    for (size_t i = 0; i < minSize; ++i) {
        auto &pos = positions[i];
        auto &projectile = projectiles[i];
        const auto &draw = drawables[i];
        if ((!pos.has_value()) || (!projectile.has_value())
            || (!draw.has_value())) {
            continue;
        }
        pos->x += static_cast<int>(
            projectile->dirX * static_cast<float>(projectile->speed));
        pos->y += static_cast<int>(
            projectile->dirY * static_cast<float>(projectile->speed));
        if ((pos->x < -100) || (pos->x > ::WINDOW_WIDTH) || (pos->y < -100)
            || (pos->y > ::WINDOW_HEIGHT)) {
            playerEvents.emplace_back(
                Event::DESTROY, static_cast<unsigned int>(i));
            registry.killEntity(static_cast<unsigned int>(i));
            continue;
        }
    }
}

void GameEngine::systemPositionProjectile(Registry &registry,
    SparseArray<Position> &positions, SparseArray<Projectile> &projectiles)
{
    const size_t minSize = min({ positions.size(), projectiles.size() });
    auto colliderEntities = registry.getEntities<Collider>();
    auto forceEntities = registry.getEntities<Force>();

    const int CELL_SIZE = 128;
    static unordered_map<int, vector<pair<Entity, Collider>>> spatialGrid;
    for (auto &[key, vec] : spatialGrid) {
        vec.clear();
    }

    for (const auto &[obstacleEntity, obstacle] : colliderEntities) {
        if (!registry.isEntityAlive(obstacleEntity))
            continue;
        auto &posOpt = registry.getComponent<Position>(obstacleEntity);
        if (!posOpt.has_value())
            continue;
        int minX = posOpt->x / CELL_SIZE;
        int maxX = (posOpt->x + obstacle.width * 2) / CELL_SIZE;
        int minY = posOpt->y / CELL_SIZE;
        int maxY = (posOpt->y + obstacle.height * 2) / CELL_SIZE;

        // Anti-lag bounds check
        if (maxX - minX > 10)
            maxX = minX + 10;
        if (maxY - minY > 10)
            maxY = minY + 10;

        for (int x = minX; x <= maxX; ++x) {
            for (int y = minY; y <= maxY; ++y) {
                spatialGrid[x + y * 1000].emplace_back(
                    obstacleEntity, obstacle);
            }
        }
    }

    for (size_t i = 0; i < minSize; ++i) {
        const auto &pos = positions[i];
        auto &projectile = projectiles[i];
        if ((!pos.has_value()) || (!projectile.has_value())) {
            continue;
        }
        if (projectile->origin == ProjectileOrigin::PLAYER) {
            continue;
        }
        const auto projectileEntity
            = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(projectileEntity)) {
            continue;
        }
        for (const auto &[forceEntity, force] : forceEntities) {
            if (!registry.isEntityAlive(forceEntity)) {
                continue;
            }
            auto &forcePosOpt = registry.getComponent<Position>(forceEntity);
            if (!forcePosOpt.has_value()) {
                continue;
            }
            auto &forceComp = registry.getComponent<Force>(forceEntity);
            if (forceComp->playerId == -1) {
                continue;
            }
            const GameplayAsset forceAsset
                = forceComp->level == 1 ? FORCE1 : FORCE2;
            if (checkCollisionWithObstacle(pos.value(), forcePosOpt.value(),
                    AssetManager::getWidth(ENEMY_PROJECTILE) * 2,
                    AssetManager::getHeight(ENEMY_PROJECTILE) * 2,
                    static_cast<int>(
                        AssetManager::getWidth(forceAsset) * 1.5f),
                    static_cast<int>(
                        AssetManager::getHeight(forceAsset) * 1.5f),
                    0, 0)) {
                playerEvents.emplace_back(Event::DESTROY,
                    static_cast<unsigned int>(projectileEntity));
                registry.killEntity(projectileEntity);
                break;
            }
        }
    }
    for (auto &[forceEntity, force] : forceEntities) {
        if (!registry.isEntityAlive(forceEntity)) {
            continue;
        }
        auto &forcePosOpt = registry.getComponent<Position>(forceEntity);
        if (!forcePosOpt.has_value()) {
            continue;
        }
        auto &forceOpt = registry.getComponent<Force>(forceEntity);
        if (!forceOpt.has_value()) {
            continue;
        }

        const GameplayAsset forceAsset
            = forceOpt->level == 1 ? FORCE1 : FORCE2;
        int fWidth
            = static_cast<int>(AssetManager::getWidth(forceAsset) * 1.5f);
        int fHeight
            = static_cast<int>(AssetManager::getHeight(forceAsset) * 1.5f);

        int pMinX = forcePosOpt->x / CELL_SIZE;
        int pMaxX = (forcePosOpt->x + fWidth) / CELL_SIZE;
        int pMinY = forcePosOpt->y / CELL_SIZE;
        int pMaxY = (forcePosOpt->y + fHeight) / CELL_SIZE;

        if (pMaxX - pMinX > 10)
            pMaxX = pMinX + 10;
        if (pMaxY - pMinY > 10)
            pMaxY = pMinY + 10;

        static vector<Entity> alreadyChecked;
        alreadyChecked.clear();

        for (int x = pMinX; x <= pMaxX; ++x) {
            for (int y = pMinY; y <= pMaxY; ++y) {
                int hash = x + y * 1000;
                if (!spatialGrid.contains(hash))
                    continue;
                for (auto &[enemyEntity, enemy] : spatialGrid[hash]) {
                    if (ranges::find(alreadyChecked, enemyEntity)
                        != alreadyChecked.end())
                        continue;
                    alreadyChecked.push_back(enemyEntity);
                    if (!registry.isEntityAlive(enemyEntity))
                        continue;
                    auto &enemyPosOpt
                        = registry.getComponent<Position>(enemyEntity);
                    if (!enemyPosOpt.has_value())
                        continue;
                    if (ranges::find(force.hitObstacles, enemyEntity)
                        != force.hitObstacles.end())
                        continue;

                    if (checkCollisionWithObstacle(forcePosOpt.value(),
                            enemyPosOpt.value(), fWidth, fHeight,
                            enemy.width * 2, enemy.height * 2, 0, 0)) {

                        auto &enemyHealthOpt
                            = registry.getComponent<Health>(enemyEntity);
                        if (enemyHealthOpt.has_value()) {
                            enemyHealthOpt->lives -= force.power;
                            if (enemyHealthOpt->lives <= 0) {
                                addToScore(100);
                                playerEvents.emplace_back(
                                    Event::SCORE_UPDATE, globalScore);
                                playerEvents.emplace_back(Event::DESTROY,
                                    static_cast<unsigned int>(enemyEntity));
                                registry.killEntity(enemyEntity);
                            }
                        }
                        force.hitObstacles.emplace_back(enemyEntity);
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < minSize; ++i) {
        const auto &pos = positions[i];
        auto &projectile = projectiles[i];
        if ((!pos.has_value()) || (!projectile.has_value())) {
            continue;
        }
        const auto projectileEntity
            = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(projectileEntity)) {
            continue;
        }
        auto &projectilePosOpt
            = registry.getComponent<Position>(projectileEntity);
        if ((!projectilePosOpt.has_value())
            || (projectile->origin != ProjectileOrigin::PLAYER)) {
            continue;
        }
        const Position &projectilePos = projectilePosOpt.value();
        const GameplayAsset projectileAsset = projectile->isCharged
            ? PLAYER_CHARGED_PROJECTILE
            : PLAYER_PROJECTILE;
        const int projectileWidth
            = AssetManager::getWidth(projectileAsset) * 2;
        const int projectileHeight
            = AssetManager::getHeight(projectileAsset) * 2;
        const int power = projectile->isCharged ? (projectile->power * 2)
                                                : projectile->power;

        int pMinX = projectilePos.x / CELL_SIZE;
        int pMaxX = (projectilePos.x + projectileWidth) / CELL_SIZE;
        int pMinY = projectilePos.y / CELL_SIZE;
        int pMaxY = (projectilePos.y + projectileHeight) / CELL_SIZE;

        if (pMaxX - pMinX > 10)
            pMaxX = pMinX + 10;
        if (pMaxY - pMinY > 10)
            pMaxY = pMinY + 10;

        static vector<Entity> alreadyCheckedProj;
        alreadyCheckedProj.clear();
        bool hit = false;

        for (int x = pMinX; x <= pMaxX && !hit; ++x) {
            for (int y = pMinY; y <= pMaxY && !hit; ++y) {
                int hash = x + y * 1000;
                if (!spatialGrid.contains(hash))
                    continue;
                for (auto &[obstacleEntity, obstacle] : spatialGrid[hash]) {
                    if (ranges::find(alreadyCheckedProj, obstacleEntity)
                        != alreadyCheckedProj.end())
                        continue;
                    alreadyCheckedProj.push_back(obstacleEntity);
                    if (!registry.isEntityAlive(obstacleEntity))
                        continue;
                    if (ranges::find(projectile->hitObstacles, obstacleEntity)
                        != projectile->hitObstacles.end()) {
                        continue;
                    }
                    auto &obstaclePosOpt
                        = registry.getComponent<Position>(obstacleEntity);
                    if (!obstaclePosOpt.has_value()) {
                        continue;
                    }
                    if (checkCollisionWithObstacle(projectilePos,
                            obstaclePosOpt.value(), projectileWidth,
                            projectileHeight, obstacle.width * 2,
                            obstacle.height * 2, 0, 0)) {
                        projectile->hitObstacles.emplace_back(obstacleEntity);

                        auto &obstacleHealthOpt
                            = registry.getComponent<Health>(obstacleEntity);
                        if (!obstacleHealthOpt.has_value()) {
                            playerEvents.emplace_back(Event::DESTROY,
                                static_cast<unsigned int>(projectileEntity));
                            registry.killEntity(projectileEntity);
                            hit = true;
                            break;
                        }
                        obstacleHealthOpt->lives -= power;
                        if (obstacleHealthOpt->lives <= 0) {
                            if (registry.getComponent<Enemy>(obstacleEntity)
                                    .has_value()) {
                                addToScore(100);
                                registry.emplaceComponent<Respawn>(
                                    obstacleEntity,
                                    steady_clock::now()
                                        + milliseconds(64 * 4));
                                registry.removeComponent<Controllable>(
                                    obstacleEntity);
                                registry.emplaceComponent<Animation>(
                                    obstacleEntity, 64, 0, 4, ENEMY_DEATH);
                                if (registry
                                        .getComponent<Boss1>(obstacleEntity)
                                        .has_value()) {
                                    boss1Defeated = true;
                                }
                            } else {
                                playerEvents.emplace_back(Event::DESTROY,
                                    static_cast<unsigned int>(obstacleEntity));
                                registry.killEntity(obstacleEntity);
                            }
                        }

                        if (!projectile->isCharged) {
                            playerEvents.emplace_back(Event::DESTROY,
                                static_cast<unsigned int>(projectileEntity));
                            registry.killEntity(projectileEntity);
                            hit = true;
                            break;
                        }
                    }
                }
            }
            if (hit)
                break;
        }
    }
}

void GameEngine::addToScore(const unsigned int points)
{
    globalScore += (points * comboMultiplier);
    comboMultiplier++;
    comboTimer = 2.0f; // 2 seconds to keep the combo going!
    playerEvents.emplace_back(Event::SCORE_UPDATE, globalScore);
}

void GameEngine::systemRespawnDrawable(Registry &registry,
    const SparseArray<Respawn> &respawns, SparseArray<Drawable> &drawables)
{
    const size_t minSize = min(respawns.size(), drawables.size());
    for (size_t i = 0; i < minSize; ++i) {
        const auto &respawn = respawns[i];
        auto &draw = drawables[i];
        if ((!respawn.has_value()) || (!draw.has_value())
            || (steady_clock::now() < respawn->respawnTime)) {
            continue;
        }
        const auto entity = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(entity)) {
            continue;
        }
        if (registry.getComponent<Player>(entity).has_value()) {
            auto &pos = registry.getComponent<Position>(entity);
            if (!pos.has_value()) {
                continue;
            }

            pos->x = ::WINDOW_WIDTH / 4;
            pos->y = ::WINDOW_HEIGHT / 2;

            draw->sprite.spritesheetIndex
                = AssetManager::getSpriteSheetId(PLAYER);
            draw->sprite.x = AssetManager::getRectX(PLAYER);
            draw->sprite.y = static_cast<unsigned short>(3 + (i * 17));
            draw->sprite.width = AssetManager::getWidth(PLAYER);
            draw->sprite.height = AssetManager::getHeight(PLAYER);

            registry.emplaceComponent<Controllable>(entity);
            registry.removeComponent<Respawn>(entity);
        } else if (registry.getComponent<Enemy>(entity).has_value()) {
            registry.killEntity(entity);
            getPlayerEvents().emplace_back(
                Event::DESTROY, static_cast<unsigned int>(entity));
        }
    }
}

void GameEngine::systemPlayerEvents(Registry &registry,
    const SparseArray<Player> &players, SparseArray<Force> &forces)
{
    const size_t minSize = players.size();
    playerLifeEvents.reserve(minSize);
    playerEvents.reserve(minSize);
    const auto colliderEntities = registry.getEntities<Collider>();
    const size_t numForces = forces.size();

    for (size_t i = 0; i < minSize; ++i) {
        const auto playerEntity
            = registry.getEntity(static_cast<unsigned int>(i));
        if (!registry.isEntityAlive(playerEntity)) {
            continue;
        }
        auto &playerPosOpt = registry.getComponent<Position>(playerEntity);
        auto &healthOpt = registry.getComponent<Health>(playerEntity);
        auto &playerOpt = registry.getComponent<Player>(playerEntity);
        const auto &controlOpt
            = registry.getComponent<Controllable>(playerEntity);
        auto &drawOpt = registry.getComponent<Drawable>(playerEntity);

        if ((!playerPosOpt.has_value()) || (!healthOpt.has_value())
            || (!controlOpt.has_value()) || (!drawOpt.has_value())
            || (!playerOpt.has_value())) {
            continue;
        }
        const Position &playerPos = playerPosOpt.value();
        const auto entityId = static_cast<unsigned int>(playerEntity);

        for (size_t j = 0; j < numForces; ++j) {
            const auto forceEntity
                = registry.getEntity(static_cast<unsigned int>(j));
            if (!registry.isEntityAlive(forceEntity)) {
                continue;
            }
            auto &forcePosOpt = registry.getComponent<Position>(forceEntity);
            auto &forceComp = forces[j];
            auto &forceAnimOpt = registry.getComponent<Animation>(forceEntity);
            if ((!forcePosOpt.has_value()) || (!forceComp.has_value())) {
                continue;
            }
            const GameplayAsset forceAsset
                = forces[j]->level == 1 ? FORCE1 : FORCE2;
            if (checkCollisionWithObstacle(playerPos, forcePosOpt.value(), 48,
                    21,
                    static_cast<int>(
                        AssetManager::getWidth(forceAsset) * 1.5f),
                    static_cast<int>(
                        AssetManager::getHeight(forceAsset) * 1.5f),
                    drawOpt->sprite.rotation, 0)) {
                auto &forceComp = forces[j];
                if (!forceComp.has_value()) {
                    continue;
                }
                if ((!forceComp->attached) && (forceComp->playerId == -1)) {
                    forceComp->attached = true;
                    playerOpt->isForceAttached = true;
                    forceComp->getTime = steady_clock::now();
                }
                forceComp->playerId = static_cast<int>(entityId);
                playerOpt->hasForce = true;

                if (registry.getComponent<Controllable>(forceEntity)
                        .has_value()) {
                    registry.removeComponent<Controllable>(forceEntity);
                }

                if (forcePosOpt->x
                    > static_cast<int>(static_cast<float>(playerPos.x)
                        + (static_cast<float>(AssetManager::getWidth(PLAYER))
                              * 1.5f)
                            / 2.0f)) {
                    playerOpt->isForceInFront = true;
                    forceComp->isForceInFront = true;
                    if (forceComp->level == 2) {
                        drawOpt->sprite.x = AssetManager::getRectX(FORCE2);
                        if (forceAnimOpt.has_value()) {
                            forceAnimOpt->asset = FORCE2;
                        }
                    }
                } else {
                    if (forceComp->level == 1) {
                        forcePosOpt->x
                            = static_cast<int>(static_cast<float>(playerPos.x)
                                - (static_cast<float>(
                                       AssetManager::getWidth(forceAsset))
                                    * 1.5f)
                                + 5.0f);
                    } else {
                        drawOpt->sprite.x
                            = AssetManager::getRectX(FORCE2_LEFT);
                        if (forceAnimOpt.has_value()) {
                            forceAnimOpt->asset = FORCE2_LEFT;
                        }
                    }
                    playerOpt->isForceInFront = false;
                    forceComp->isForceInFront = false;
                }
                if (forceComp->level == 1) {
                    forcePosOpt->y = playerPos.y;
                } else {
                    forcePosOpt->y = playerPos.y - 5;
                }
            }
        }

        if (checkCollisionWithGround(playerPos)) {
            --healthOpt->lives;
            if (healthOpt->lives > 0) {
                registry.removeComponent<Controllable>(playerEntity);
                registry.emplaceComponent<Respawn>(
                    playerEntity, steady_clock::now() + milliseconds(64 * 7));
                playerLifeEvents.emplace_back(entityId, healthOpt->lives);
            } else {
                playerLifeEvents.emplace_back(entityId, 0);
                playerEvents.emplace_back(Event::DESTROY, entityId);
                registry.killEntity(playerEntity);
                continue;
            }
        }
        for (const auto &[obstacleEntity, obstacle] : colliderEntities) {
            if (!registry.isEntityAlive(obstacleEntity)) {
                continue;
            }
            auto &obstaclePosOpt
                = registry.getComponent<Position>(obstacleEntity);
            if (!obstaclePosOpt.has_value()) {
                continue;
            }
            if (checkCollisionWithObstacle(playerPos, obstaclePosOpt.value(),
                    48, 21, obstacle.width * 2, obstacle.height * 2,
                    drawOpt->sprite.rotation, 0)) {
                --healthOpt->lives;
                if (healthOpt->lives > 0) {
                    registry.removeComponent<Controllable>(playerEntity);
                    registry.emplaceComponent<Respawn>(playerEntity,
                        steady_clock::now() + milliseconds(64 * 7));
                    playerLifeEvents.emplace_back(entityId, healthOpt->lives);
                    detachForcesFromPlayer(registry, playerEntity);
                } else {
                    playerLifeEvents.emplace_back(entityId, 0);
                    playerEvents.emplace_back(Event::DESTROY, entityId);
                    registry.killEntity(playerEntity);
                    break;
                }
            }
        }
    }
}

void GameEngine::systemMob1DrawableControllable(const Registry & /*unused*/,
    const SparseArray<Mob1> &mobs, SparseArray<Drawable> &drawables,
    SparseArray<Controllable> &controllables) const
{
    const size_t minSize
        = min({ mobs.size(), drawables.size(), controllables.size() });
    if (minSize == 0) {
        return;
    }
    static constexpr float AMPLITUDE = 5.0f;
    static constexpr float FREQUENCY = 0.5f;
    static constexpr float PHASE_SHIFT = 1.0f;
    static constexpr float RAD_TO_DEG = 180.0f / static_cast<float>(M_PI);
    static float mobTime = 0.0f;

    for (size_t i = 0; i < minSize; ++i) {
        auto &mob = mobs[i];
        auto &draw = drawables[i];
        auto &control = controllables[i];

        if ((!mob.has_value()) || (!draw.has_value())
            || (!control.has_value())) {
            continue;
        }
        const float timePhase
            = (FREQUENCY * mobTime) + (static_cast<float>(i) * PHASE_SHIFT);
        const float verticalSpeed = AMPLITUDE * FREQUENCY * cos(timePhase);
        const float rotation
            = (atan2(verticalSpeed, -1.0f) * RAD_TO_DEG) - 180.0f;

        draw->sprite.rotation = static_cast<unsigned short>(rotation);
        control->up = (verticalSpeed < 0);
        control->down = (verticalSpeed > 0);
    }
    mobTime += 0.1f;
}

void GameEngine::systemMob1Position(Registry &registry,
    SparseArray<Mob1> &mobs, SparseArray<Position> &positions)
{
    const float currentTime = getElapsedTime();
    const size_t systemSize = min(mobs.size(), positions.size());
    if (systemSize == 0) {
        return;
    }
    for (size_t i = 0; i < systemSize; ++i) {
        auto &mob = mobs[i];
        auto &pos = positions[i];

        if ((!mob.has_value()) || (!pos.has_value())) {
            continue;
        }
        if ((currentTime - mob->lastShootTime) < mob->shootCooldown) {
            continue;
        }
        const unsigned short projectileWidth
            = AssetManager::getWidth(ENEMY_PROJECTILE);
        const unsigned short projectileHeight
            = AssetManager::getHeight(ENEMY_PROJECTILE);
        const Position targetPos = findNearestPlayer(pos->x, pos->y);
        const auto dx = static_cast<float>(targetPos.x - pos->x);
        const auto dy = static_cast<float>(targetPos.y - pos->y);
        const float length = sqrt((dx * dx) + (dy * dy));
        const float dirX = dx / length;
        const float dirY = dy / length;
        const auto projectileEntity
            = make_shared<Entity>(registry.spawnEntity());
        if (!registry.isEntityAlive(*projectileEntity)) {
            return;
        }
        const float projectilePosX
            = static_cast<float>(pos->x) + static_cast<float>(projectileWidth);
        const float projectilePosY = static_cast<float>(pos->y)
            + (static_cast<float>(projectileHeight) / 2.0f);
        registry.emplaceComponent<Animation>(
            *projectileEntity, 64, 4, 0, ENEMY_PROJECTILE);
        registry.emplaceComponent<Collider>(
            *projectileEntity, projectileWidth, projectileHeight, 2.0f, 2.0f);
        registry.emplaceComponent<Position>(*projectileEntity,
            static_cast<int>(projectilePosX),
            static_cast<int>(projectilePosY));
        registry.emplaceComponent<Projectile>(*projectileEntity,
            (currentLevel == 1) ? 5 : 8, false, dirX, dirY,
            ProjectileOrigin::ENEMY);
        registry.emplaceComponent<Health>(*projectileEntity, 1);
        registry.emplaceComponent<Drawable>(*projectileEntity,
            Sprite { AssetManager::getSpriteSheetId(ENEMY_PROJECTILE),
                AssetManager::getRectX(ENEMY_PROJECTILE),
                AssetManager::getRectY(ENEMY_PROJECTILE), projectileWidth,
                projectileHeight, static_cast<int>(projectilePosX),
                static_cast<int>(projectilePosY), 2.0f, 2.0f,
                static_cast<unsigned int>(*projectileEntity), 0 });
        mob->lastShootTime = currentTime;
    }
}

void GameEngine::systemBossMovement(Registry &registry,
    SparseArray<Boss1> &bosses, SparseArray<Position> &positions)
{
    const float currentTime = getElapsedTime();
    const size_t systemSize = min(bosses.size(), positions.size());
    for (size_t i = 0; i < systemSize; ++i) {
        auto &boss = bosses[i];
        auto &pos = positions[i];
        if ((!boss.has_value()) || (!pos.has_value())) {
            continue;
        }

        float amplitude = 200.0f;
        float frequency = 1.5f;

        // Boss Phase 2 (Enrage Mode)
        const auto entityId = static_cast<unsigned int>(i);
        auto &healthOpt
            = registry.getComponent<Health>(registry.getEntity(entityId));
        if (healthOpt.has_value() && healthOpt->lives < 500) {
            amplitude = 250.0f;
            frequency = 3.5f; // Bouge beaucoup plus vite
        }

        float baseHeight = ::WINDOW_HEIGHT / 2.0f;
        pos->y = static_cast<int>(
            baseHeight + sin(currentTime * frequency) * amplitude);
    }
}

void GameEngine::systemBoss1Position(Registry &registry,
    SparseArray<Boss1> &boss1, SparseArray<Position> &positions)
{
    const float currentTime = getElapsedTime();
    const size_t systemSize = min(boss1.size(), positions.size());
    if (systemSize == 0) {
        return;
    }
    for (size_t i = 0; i < systemSize; ++i) {
        auto &mob = boss1[i];
        auto &pos = positions[i];

        if ((!mob.has_value()) || (!pos.has_value())) {
            continue;
        }

        // Boss Phase 2 (Enrage Mode)
        const auto entityId = static_cast<unsigned int>(i);
        auto &healthOpt
            = registry.getComponent<Health>(registry.getEntity(entityId));
        float currentCooldown = mob->shootCooldown;
        if (healthOpt.has_value() && healthOpt->lives < 500) {
            currentCooldown = 0.5f; // Tire 2x plus vite (de base 1.0f)
        }

        if ((currentTime - mob->lastShootTime) < currentCooldown) {
            continue;
        }
        const unsigned short projectileWidth
            = AssetManager::getWidth(BOSS1_PROJECTILE);
        const unsigned short projectileHeight
            = AssetManager::getHeight(BOSS1_PROJECTILE);
        const auto projectilePosX = static_cast<int>(
            (static_cast<float>(pos->x)
                + static_cast<float>(AssetManager::getWidth(BOSS1)))
            - (projectileWidth / 2.0f));
        const auto projectilePosY = static_cast<int>(
            (static_cast<float>(pos->y)
                + static_cast<float>(AssetManager::getHeight(BOSS1)))
            - (projectileHeight / 2.0f));
        const Position targetPos
            = findNearestPlayer(projectilePosX, projectilePosY);
        const auto dx
            = static_cast<float>((targetPos.x + 24) - projectilePosX);
        const auto dy
            = static_cast<float>((targetPos.y + 10.5) - projectilePosY);
        const float length = sqrt((dx * dx) + (dy * dy));
        const float dirX = dx / length;
        const float dirY = dy / length;
        const auto projectileEntity
            = make_shared<Entity>(registry.spawnEntity());
        if (!registry.isEntityAlive(*projectileEntity)) {
            return;
        }
        registry.emplaceComponent<Animation>(
            *projectileEntity, 64, 4, 0, BOSS1_PROJECTILE);
        registry.emplaceComponent<Collider>(
            *projectileEntity, projectileWidth, projectileHeight, 2.0f, 2.0f);
        registry.emplaceComponent<Position>(
            *projectileEntity, projectilePosX, projectilePosY);
        registry.emplaceComponent<Projectile>(*projectileEntity,
            (currentLevel == 1) ? 5 : 8, false, dirX, dirY,
            ProjectileOrigin::BOSS1);
        registry.emplaceComponent<Health>(*projectileEntity, 1);
        registry.emplaceComponent<Drawable>(*projectileEntity,
            Sprite { AssetManager::getSpriteSheetId(BOSS1_PROJECTILE),
                AssetManager::getRectX(BOSS1_PROJECTILE),
                AssetManager::getRectY(BOSS1_PROJECTILE), projectileWidth,
                projectileHeight, projectilePosX, projectilePosY, 2.0f, 2.0f,
                static_cast<unsigned int>(*projectileEntity), 0 });
        mob->lastShootTime = currentTime;
    }
}

void GameEngine::systemTurretPosition(Registry &registry,
    SparseArray<Turret> &turrets, SparseArray<Position> &positions)
{
    const float currentTime = getElapsedTime();
    const size_t systemSize = min(turrets.size(), positions.size());
    if (systemSize == 0) {
        return;
    }
    for (size_t i = 0; i < systemSize; ++i) {
        auto &turret = turrets[i];
        auto &pos = positions[i];

        if ((!turret.has_value()) || (!pos.has_value())) {
            continue;
        }
        if ((currentTime - turret->lastShootTime) < turret->shootCooldown) {
            continue;
        }
        const unsigned short projectileWidth
            = AssetManager::getWidth(ENEMY_PROJECTILE);
        const unsigned short projectileHeight
            = AssetManager::getHeight(ENEMY_PROJECTILE);
        const Position targetPos = findNearestPlayer(pos->x, pos->y);
        const auto dx = static_cast<float>(targetPos.x - pos->x);
        const auto dy = static_cast<float>(targetPos.y - pos->y);
        const float length = sqrt((dx * dx) + (dy * dy));
        const float dirX = dx / length;
        const float dirY = dy / length;
        const auto projectileEntity
            = make_shared<Entity>(registry.spawnEntity());
        if (!registry.isEntityAlive(*projectileEntity)) {
            return;
        }
        const float projectilePosX
            = static_cast<float>(pos->x) + static_cast<float>(projectileWidth);
        const float projectilePosY = static_cast<float>(pos->y)
            + (static_cast<float>(projectileHeight) / 2.0f);
        registry.emplaceComponent<Animation>(
            *projectileEntity, 64, 4, 0, ENEMY_PROJECTILE);
        registry.emplaceComponent<Collider>(
            *projectileEntity, projectileWidth, projectileHeight, 2.0f, 2.0f);
        registry.emplaceComponent<Position>(*projectileEntity,
            static_cast<int>(projectilePosX),
            static_cast<int>(projectilePosY));
        registry.emplaceComponent<Projectile>(*projectileEntity,
            (currentLevel == 1) ? 5 : 8, false, dirX, dirY,
            ProjectileOrigin::ENEMY);
        registry.emplaceComponent<Health>(*projectileEntity, 1);
        registry.emplaceComponent<Drawable>(*projectileEntity,
            Sprite { AssetManager::getSpriteSheetId(ENEMY_PROJECTILE),
                AssetManager::getRectX(ENEMY_PROJECTILE),
                AssetManager::getRectY(ENEMY_PROJECTILE), projectileWidth,
                projectileHeight, static_cast<int>(projectilePosX),
                static_cast<int>(projectilePosY), 2.0f, 2.0f,
                static_cast<unsigned int>(*projectileEntity), 0 });
        turret->lastShootTime = currentTime;
    }
}
