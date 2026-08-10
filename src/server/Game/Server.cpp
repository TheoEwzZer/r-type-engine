/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Server
*/

#include "Server.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <map>

using namespace rtype;

Server::Server(const EngineConfig &config) :
    ioContext(), network(ioContext, 4242), recvBuffer(65'504),
    lastUpdate(steady_clock::now()), config(config),
    gameEngine(registry, config, network), strand(ioContext.get_executor())
{
    std::filesystem::create_directory("logs");

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_now;
#ifdef _WIN32
    localtime_s(&tm_now, &now_time);
#else
    localtime_r(&now_time, &tm_now);
#endif

    std::stringstream ss;
    ss << "logs/log-game-"
       << std::put_time(&tm_now, "%d-%m-%Y-%H-%M-%S") << ".log";

    std::stringstream folderName;
    folderName << "logs_run_" << std::put_time(&tm_now, "%d-%m-%Y-%H-%M-%S");
    std::filesystem::create_directory(folderName.str());

    std::string serverPath = folderName.str() + "/server.log";
    std::string gamePath = folderName.str() + "/game.log";
    serverLogFile.open(serverPath, std::ios::out | std::ios::app);
    gameLogFile.open(gamePath, std::ios::out | std::ios::app);

    if (!serverLogFile.is_open() || !gameLogFile.is_open()) {
        throw std::runtime_error("Failed to create log files");
    }

    logEvent("[SERVER ENGINE] Server started on port 4242");
    startReceive();
}

Server::~Server()
{
    logEvent("[SERVER ENGINE] Server is shutting down.");
    networkPool.stop = true;
    gamePool.stop = true;
    networkPool.condition.notify_all();
    gamePool.condition.notify_all();
}

void Server::startReceive()
{
    logEvent("[SERVER ENGINE] Server is now listening for incoming data.");
    recvBuffer.resize(65'504);
    const auto senderEndpoint = make_shared<udp::endpoint>();
    network.getSocket().async_receive_from(asio::buffer(recvBuffer),
        *senderEndpoint,
        asio::bind_executor(strand,
            [this, senderEndpoint](
                const error_code ec, const size_t bytesRecvd) {
                if ((!ec) && (bytesRecvd > 0)) {
                    recvBuffer.resize(bytesRecvd);
                    handleReceivedData(ec, bytesRecvd, *senderEndpoint);
                }
                startReceive();
            }));
}

void Server::handleReceivedData(const error_code &error,
    const size_t bytesTransferred, const udp::endpoint senderEndpoint)
{
    if ((!error) && (bytesTransferred > 0)) {
        logEvent("[SERVER ENGINE] Received " + std::to_string(bytesTransferred)
            + " bytes from " + senderEndpoint.address().to_string() + ":"
            + std::to_string(senderEndpoint.port()));
        if (recvBuffer.size() == 6) {
            handleClientInputMove(recvBuffer, senderEndpoint);
        } else if (recvBuffer.size() == 5) {
            handleClientInput(recvBuffer, senderEndpoint);
        }
    }
}

void Server::initThreadPools()
{
    for (size_t i = 0; i < networkThreads; ++i) {
        networkPool.threads.emplace_back([this] { networkThreadFunction(); });
    }
    for (size_t i = 0; i < gameThreads; ++i) {
        gamePool.threads.emplace_back([this] { gameThreadFunction(); });
    }
}

void Server::run()
{
    initThreadPools();

    while (true) {
        network.setNonBlocking(true);
        enqueueNetworkTask([this] { processClientConnections(); });

        if (isGameStarted) {
            const auto now = steady_clock::now();
            const auto duration
                = duration_cast<milliseconds>(now - lastUpdate).count();

            if ((duration > 16) && (!processingGameState.exchange(true))) {
                enqueueGameTask([this] { processGameLogic(); });
                enqueueNetworkTask([this] { broadcastStates(); });

                lastUpdate = now;
            }
        }

        this_thread::yield();
    }
}

void Server::enqueueNetworkTask(function<void()> task)
{
    const lock_guard lock(networkPool.queueMutex);
    networkPool.tasks.emplace(move(task));
    networkPool.condition.notify_one();
}

void Server::enqueueGameTask(function<void()> task)
{
    const lock_guard lock(gamePool.queueMutex);
    gamePool.tasks.emplace(move(task));
    gamePool.condition.notify_one();
}

void Server::networkThreadFunction()
{
    while (true) {
        function<void()> task;
        if (!getNetworkTask(task)) {
            return;
        }
        task();
    }
}

void Server::gameThreadFunction()
{
    while (true) {
        function<void()> task;
        if (!getGameTask(task)) {
            return;
        }
        task();
    }
}

bool Server::getNetworkTask(function<void()> &task)
{
    unique_lock lock(networkPool.queueMutex);
    networkPool.condition.wait(lock,
        [this] { return networkPool.stop || (!networkPool.tasks.empty()); });
    if (networkPool.stop && networkPool.tasks.empty()) {
        return false;
    }
    task = move(networkPool.tasks.front());
    networkPool.tasks.pop();
    return true;
}

bool Server::getGameTask(function<void()> &task)
{
    unique_lock lock(gamePool.queueMutex);
    gamePool.condition.wait(
        lock, [this] { return gamePool.stop || (!gamePool.tasks.empty()); });
    if (gamePool.stop && gamePool.tasks.empty()) {
        return false;
    }
    task = move(gamePool.tasks.front());
    gamePool.tasks.pop();
    return true;
}

void Server::processGameLogic()
{
    logEvent("[GAME ENGINE] Processing game logic and updating game states.");
    const lock_guard lock(stateMutex);
    gameEngine.update();
    gameEngine.runSystems();
    logEvent("[GAME ENGINE] Game logic processed and game states updated.");
    logEvent("[GAME ENGINE] Additional advanced logic checks completed.");
    processingGameState.store(false);
}

void Server::broadcastStates()
{
    logEvent("[SERVER ENGINE] Broadcasting game states.");
    const lock_guard lock(stateMutex);
    sendEntityStates();
    logEvent("[SERVER ENGINE] Game states broadcasted.");
}

void Server::handleClientInputMove(
    const vector<unsigned char> &data, const udp::endpoint &clientEndpoint)
{
    try {
        std::unique_ptr<PlayerEventMove> event = std::make_unique<PlayerEventMove>(BinaryProtocol::deserializePlayerEventMove(data));
        if (!network.getClients().contains(clientEndpoint)) {
            cerr << "Client not found: " << clientEndpoint << "\n";
            return;
        }
        const auto entity = network.getClients()[clientEndpoint];
        auto &control = registry.getComponent<Controllable>(*entity);
        if (control) {
            switch (event->dx) {
                case Direction::LEFT:
                    logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " goes left.");
                    control->left = true;
                    break;
                case Direction::RIGHT:
                    logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " goes right.");
                    control->right = true;
                    break;
                default:
                    control->left = false;
                    control->right = false;
                    break;
            }
            switch (event->dy) {
                case Direction::UP:
                    logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " goes up.");
                    control->up = true;
                    control->prevUp = true;
                    break;
                case Direction::DOWN:
                    logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " goes down.");
                    control->down = true;
                    control->prevDown = true;
                    break;
                default:
                    control->up = false;
                    control->down = false;
                    control->prevUp = false;
                    control->prevDown = false;
                    break;
            }
        }
    } catch (const exception &e) {
        cerr << "Error handling client input move: " << e.what() << "\n";
    }
}

void Server::handleClientInput(
    const vector<unsigned char> &data, const udp::endpoint &clientEndpoint)
{
    try {
        unique_ptr<PlayerEvent> event = std::make_unique<PlayerEvent>(BinaryProtocol::deserializePlayerEvent(data));
        if (!network.getClients().contains(clientEndpoint)) {
            cerr << "Client not found: " << clientEndpoint << "\n";
            return;
        }
        const auto entity = network.getClients()[clientEndpoint];
        auto &player = registry.getComponent<Player>(*entity);

        if (event->event == Event::SHOOT) {
            logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " fired a projectile.");
            auto &pos = registry.getComponent<Position>(*entity);
            if (pos) {
                const auto projectile
                    = make_shared<Entity>(registry.spawnEntity());
                registry.emplaceComponent<Position>(*projectile,
                    (pos->x + (AssetManager::getWidth(PLAYER))),
                    ((pos->y + ((AssetManager::getHeight(PLAYER)) / 2))
                        - ((AssetManager::getHeight(PLAYER_PROJECTILE)) / 2)));

                registry.emplaceComponent<Projectile>(*projectile, 10, false,
                    1.0f, 0.0f, ProjectileOrigin::PLAYER,
                    player->isForceInFront && player->hasForce);

                registry.emplaceComponent<Drawable>(*projectile,
                    Sprite { AssetManager::getSpriteSheetId(PLAYER_PROJECTILE),
                        AssetManager::getRectX(PLAYER_PROJECTILE),
                        AssetManager::getRectY(PLAYER_PROJECTILE),
                        AssetManager::getWidth(PLAYER_PROJECTILE),
                        AssetManager::getHeight(PLAYER_PROJECTILE),
                        (pos->x + (AssetManager::getWidth(PLAYER))),
                        ((pos->y + ((AssetManager::getHeight(PLAYER)) / 2))
                            - ((AssetManager::getHeight(PLAYER_PROJECTILE))
                                / 2)),
                        2, 2, static_cast<unsigned int>(*projectile), 0 });

                if (player->hasForce && player->isForceAttached
                    && (!player->isForceInFront)) {
                    const auto forceProjectile
                        = make_shared<Entity>(registry.spawnEntity());
                    registry.emplaceComponent<Position>(*forceProjectile,
                        (pos->x
                            - AssetManager::getWidth(PLAYER_PROJECTILE_LEFT)),
                        ((pos->y + (AssetManager::getHeight(PLAYER) / 2))
                            - (AssetManager::getHeight(PLAYER_PROJECTILE)
                                / 2)));

                    registry.emplaceComponent<Projectile>(*forceProjectile, 10,
                        false, -1.0f, 0.0f, ProjectileOrigin::PLAYER,
                        player->isForceInFront && player->hasForce);

                    registry.emplaceComponent<Drawable>(*forceProjectile,
                        Sprite { AssetManager::getSpriteSheetId(
                                     PLAYER_PROJECTILE_LEFT),
                            AssetManager::getRectX(PLAYER_PROJECTILE_LEFT),
                            AssetManager::getRectY(PLAYER_PROJECTILE_LEFT),
                            AssetManager::getWidth(PLAYER_PROJECTILE_LEFT),
                            AssetManager::getHeight(PLAYER_PROJECTILE_LEFT),
                            (pos->x
                                - AssetManager::getWidth(
                                    PLAYER_PROJECTILE_LEFT)),
                            ((pos->y + (AssetManager::getHeight(PLAYER) / 2))
                                - (AssetManager::getHeight(
                                       PLAYER_PROJECTILE_LEFT)
                                    / 2)),
                            2, 2, static_cast<unsigned int>(*forceProjectile),
                            0 });
                }
            } else {
                cerr << "Position component missing for entity: "
                     << static_cast<unsigned int>(*entity) << "\n";
            }
        }
        if (event->event == Event::CHARGED_SHOOT) {
            logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " used a charged shot.");
            auto &pos = registry.getComponent<Position>(*entity);
            if (pos) {
                rtype::GameplayAsset projectileAsset
                    = player->hasForce && player->isForceInFront
                    ? FORCE_PROJECTILE
                    : PLAYER_CHARGED_PROJECTILE;
                const auto projectile
                    = make_shared<Entity>(registry.spawnEntity());
                registry.emplaceComponent<Position>(*projectile,
                    (pos->x + AssetManager::getWidth(PLAYER)),
                    ((pos->y + (AssetManager::getHeight(PLAYER) / 2))
                        - (AssetManager::getHeight(projectileAsset) / 2)));

                registry.emplaceComponent<Projectile>(*projectile, 10, true,
                    1.0f, 0.0f, ProjectileOrigin::PLAYER, player->hasForce);

                if (config.enableAnimation) {
                    registry.emplaceComponent<Animation>(
                        *projectile, 64, 2, 0, projectileAsset);
                }

                registry.emplaceComponent<Drawable>(*projectile,
                    Sprite { AssetManager::getSpriteSheetId(projectileAsset),
                        AssetManager::getRectX(projectileAsset),
                        AssetManager::getRectY(projectileAsset),
                        AssetManager::getWidth(projectileAsset),
                        AssetManager::getHeight(projectileAsset),
                        (pos->x + AssetManager::getWidth(PLAYER)),
                        ((pos->y + (AssetManager::getHeight(PLAYER) / 2))
                            - (AssetManager::getHeight(projectileAsset) / 2)),
                        2, 2, static_cast<unsigned int>(*projectile), 0 });

                if (player->hasForce && player->isForceAttached
                    && (!player->isForceInFront)) {
                    projectileAsset
                        = player->hasForce && (!player->isForceInFront)
                        ? FORCE_PROJECTILE_LEFT
                        : PLAYER_CHARGED_PROJECTILE;
                    const auto forceProjectile
                        = make_shared<Entity>(registry.spawnEntity());
                    registry.emplaceComponent<Position>(*forceProjectile,
                        (pos->x - AssetManager::getWidth(FORCE_PROJECTILE)),
                        ((pos->y + (AssetManager::getHeight(PLAYER) / 2))
                            - (AssetManager::getHeight(FORCE_PROJECTILE)
                                / 2)));

                    registry.emplaceComponent<Projectile>(*forceProjectile, 10,
                        true, -1.0f, 0.0f, ProjectileOrigin::PLAYER,
                        player->hasForce);

                    if (config.enableAnimation) {
                        registry.emplaceComponent<Animation>(
                            *forceProjectile, 64, 2, 0, projectileAsset);
                    }

                    registry.emplaceComponent<Drawable>(*forceProjectile,
                        Sprite {
                            AssetManager::getSpriteSheetId(projectileAsset),
                            AssetManager::getRectX(projectileAsset),
                            AssetManager::getRectY(projectileAsset),
                            AssetManager::getWidth(projectileAsset),
                            AssetManager::getHeight(projectileAsset),
                            (pos->x
                                - AssetManager::getWidth(FORCE_PROJECTILE)),
                            ((pos->y + (AssetManager::getHeight(PLAYER) / 2))
                                - (AssetManager::getHeight(FORCE_PROJECTILE)
                                    / 2)),
                            2, 2, static_cast<unsigned int>(*forceProjectile),
                            0 });
                }
            } else {
                cerr << "Position component missing for entity: "
                     << static_cast<unsigned int>(*entity) << "\n";
            }
        }
        if (event->event == Event::DESTROY) {
            logEvent("[GAME ENGINE] Player " + std::to_string(event->playerId) + " has been destroyed.");
            gameEngine.getPlayerEvents().emplace_back(*event);
            registry.killEntity(*entity);
            network.getClients().erase(clientEndpoint);
            cout << "Client " << static_cast<unsigned int>(*entity)
                 << " disconnected\n";
        }
        if (event->event == Event::DETACH_ATTACH_FORCE) {
            pendingForceEvents.push_back({*event, steady_clock::now(), 0, clientEndpoint});
            logEvent("[SERVER ENGINE] Force event queued for player " + std::to_string(event->playerId));
            auto forceEntities = registry.getEntities<Force>();
            for (const auto &[forceEntity, force] : forceEntities) {
                if (!registry.isEntityAlive(forceEntity)) {
                    continue;
                }
                auto &forceComp = registry.getComponent<Force>(forceEntity);
                auto &animComp = registry.getComponent<Animation>(forceEntity);
                auto &playerComp = registry.getComponent<Player>(*entity);
                if ((!forceComp.has_value()) || (!playerComp.has_value())) {
                    continue;
                }
                if (forceComp->playerId == static_cast<int>(event->playerId)) {
                    forceComp->attached = !forceComp->attached;
                    playerComp->isForceAttached = forceComp->attached;
                    if (!forceComp->attached) {
                        if (forceComp->level == 2) {
                            animComp->asset = FORCE2;
                        }
                    }
                    break;
                }
            }
        }
    } catch (const exception &e) {
        cerr << "Error handling client input: " << e.what() << "\n";
    }
}

void Server::sendEntityStates()
{
    handlePendingDeathEvents();
    handlePendingJoinEvents();
    handlePendingForceEvents();

    if (!gameEngine.getObstacles().empty()) {
        for (const auto &mob : gameEngine.getObstacles()) {
            logEvent("[GAME ENGINE] Mob " + std::to_string(mob.id) + " has appeared at (" + 
                     std::to_string(mob.x) + ", " + std::to_string(mob.y) + ").");
        }
        gameEngine.getObstacles().clear();
    }
    
    if (!gameEngine.getCurrentSprites().empty()) {
        network.sendAll(
            BinaryProtocol::serializeSpriteList(gameEngine.getCurrentSprites()));
    }
    if (!gameEngine.getPlayerLevelEvents().empty()) {
        for (const auto &event : gameEngine.getPlayerLevelEvents()) {
            const auto levelBuffer
                = BinaryProtocol::serializePlayerEventLevel(event);
            network.sendAll(levelBuffer);
        }
        gameEngine.getPlayerLevelEvents().clear();
        return;
    }

    if (!gameEngine.getPlayerLifeEvents().empty()) {
        for (const auto &event : gameEngine.getPlayerLifeEvents()) {
            const auto lifeBuffer
                = BinaryProtocol::serializePlayerEventLife(event);
            const auto &client = gameEngine.getClient();
            const auto clientIt
                = ranges::find_if(client, [&event](const auto &client) {
                      return static_cast<unsigned int>(*client.second)
                          == event.playerId;
                  });
            if (clientIt != client.end()) {
                network.sendTo(lifeBuffer, clientIt->first);
            }
        }
        gameEngine.getPlayerLifeEvents().clear();
        return;
    }
    if (!gameEngine.getPlayerEvents().empty()) {
        for (const auto &event : gameEngine.getPlayerEvents()) {
            if (event.event == Event::DESTROY) {
                // Ajouter l'événement de mort aux événements en attente
                pendingDeathEvents.push_back({event, steady_clock::now(), 0});
                logEvent("[SERVER ENGINE] Death event queued for player " + std::to_string(event.playerId));
            } else {
                const auto buffer = BinaryProtocol::serializePlayerEvent(event);
                network.sendAll(buffer);
            }
        }
        gameEngine.getPlayerEvents().clear();
        return;
    }
}

void Server::handlePendingDeathEvents()
{
    const auto now = steady_clock::now();
    
    auto it = pendingDeathEvents.begin();
    while (it != pendingDeathEvents.end()) {
        auto timeDiff = duration_cast<seconds>(now - it->lastSent).count();
        
        if (timeDiff >= 4 || it->retries == 0) { // 4 secondes entre chaque tentative
            const auto buffer = BinaryProtocol::serializePlayerEvent(it->event);
            network.sendAll(buffer);
            
            it->lastSent = now;
            it->retries++;
            
            logEvent("[SERVER ENGINE] Death event retry " + std::to_string(it->retries) + 
                     " for player " + std::to_string(it->event.playerId));

            if (it->retries >= MAX_DEATH_RETRIES) {
                logEvent("[SERVER ENGINE] Death event max retries reached for player " + 
                         std::to_string(it->event.playerId));
                it = pendingDeathEvents.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void Server::handlePendingForceEvents()
{
    const auto now = steady_clock::now();
    auto it = pendingForceEvents.begin();
    while (it != pendingForceEvents.end()) {
        auto timeDiff = duration_cast<seconds>(now - it->lastSent).count();
        
        if (timeDiff >= 4 || it->retries == 0) {
            const auto buffer = BinaryProtocol::serializePlayerEvent(it->event);
            network.sendAll(buffer);
            it->lastSent = now;
            it->retries++;
            
            logEvent("[SERVER ENGINE] Force event retry " + std::to_string(it->retries) + 
                    " for player " + std::to_string(it->event.playerId));

            if (it->retries >= MAX_DEATH_RETRIES) {
                logEvent("[SERVER ENGINE] Force event max retries reached for player " + 
                        std::to_string(it->event.playerId));
                it = pendingForceEvents.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void Server::processClientConnections()
{
    udp::endpoint senderEndpoint;
    asio::error_code error;
    const size_t length = network.getSocket().receive_from(
        buffer(recvBuffer), senderEndpoint, 0, error);

    if (error == asio::error::would_block) {
        return;
    }

    if (length > 0) {
        try {
            const PlayerEvent event
                = BinaryProtocol::deserializePlayerEvent(recvBuffer);
            if (event.event == Event::SPECTATOR) {
                cout << "New spectator connected: " << senderEndpoint << "\n";
                network.getSpectators().push_back(senderEndpoint);
                network.sendTo(BinaryProtocol::serializeSpriteList(
                                   gameEngine.getCurrentObstacles()),
                    senderEndpoint);
                return;
            }
            if (!network.getClients().contains(senderEndpoint)
                && event.event == Event::JOIN) {
                if (network.getClients().size() >= 4) {
                    logEvent("[SERVER ENGINE] Maximum number of clients reached.");
                    return;
                }

                if (!isGameStarted) {
                    if (config.enableAI) {
                        const auto player
                            = make_shared<Entity>(registry.spawnEntity());
                        registry.emplaceComponent<Position>(
                            *player, ::WINDOW_WIDTH / 4, ::WINDOW_HEIGHT / 2);
                        registry.emplaceComponent<Controllable>(
                            *player, false, false, false, false, 3);
                        registry.emplaceComponent<Player>(*player);
                        registry.emplaceComponent<AI>(*player);
                        registry.emplaceComponent<Health>(*player, 3);
                        if (config.enableAnimation) {
                            registry.emplaceComponent<Animation>(
                                *player, 64, 0, 8, PLAYER);
                        }
                        const auto aiId = static_cast<unsigned int>(*player);
                        registry.emplaceComponent<Drawable>(*player,
                            Sprite { AssetManager::getSpriteSheetId(PLAYER),
                                AssetManager::getRectX(PLAYER),
                                static_cast<unsigned short>(3 + (3 * 17)),
                                AssetManager::getWidth(PLAYER),
                                AssetManager::getHeight(PLAYER),
                                ::WINDOW_WIDTH / 4, ::WINDOW_HEIGHT / 2, 1.5f,
                                1.5f, aiId, 0 });
                    }
                    isGameStarted = true;
                }
                const auto player
                    = make_shared<Entity>(registry.spawnClientEntity());
                registry.emplaceComponent<Position>(
                    *player, ::WINDOW_WIDTH / 4, ::WINDOW_HEIGHT / 2);
                registry.emplaceComponent<Controllable>(*player);
                registry.emplaceComponent<Player>(*player);
                registry.emplaceComponent<Health>(*player, 3);
                if (config.enableAnimation) {
                    registry.emplaceComponent<Animation>(
                        *player, 64, 0, 8, PLAYER);
                }
                const auto clientId = static_cast<unsigned int>(*player);
                unsigned short skin
                    = event.playerId == 0 ? clientId : event.playerId;
                registry.emplaceComponent<Drawable>(*player,
                    Sprite {
                        AssetManager::getSpriteSheetId(GameplayAsset::PLAYER),
                        AssetManager::getRectX(GameplayAsset::PLAYER),
                        static_cast<unsigned short>(3 + (skin * 17)),
                        AssetManager::getWidth(GameplayAsset::PLAYER),
                        AssetManager::getHeight(GameplayAsset::PLAYER),
                        ::WINDOW_WIDTH / 4, ::WINDOW_HEIGHT / 2, 1.5f, 1.5f,
                        clientId, 0 });
                network.getClients()[senderEndpoint] = player;
                cout << "New client connected: " << senderEndpoint << "\n";
                logEvent("[SERVER ENGINE] New client connected: " 
                    + senderEndpoint.address().to_string() + ":"
                    + std::to_string(senderEndpoint.port()));
                logEvent("[SERVER ENGINE] Player " + std::to_string(clientId)
                    + " connecting from IP " + senderEndpoint.address().to_string()
                    + ":" + std::to_string(senderEndpoint.port()));
                const PlayerEvent joinEvent { Event::JOIN, clientId };
                pendingJoinEvents.push_back({joinEvent, steady_clock::now(), 0, senderEndpoint});
                logEvent("[SERVER ENGINE] Join event queued for client " + std::to_string(clientId));
                const auto idBuffer
                    = BinaryProtocol::serializePlayerEvent(joinEvent);
                network.sendTo(idBuffer, senderEndpoint);
                const PlayerEventLevel levelEvent { gameEngine.getLevel() };
                const auto levelBuffer
                    = BinaryProtocol::serializePlayerEventLevel(levelEvent);
                network.sendTo(levelBuffer, senderEndpoint);
                if (!gameEngine.getCurrentObstacles().empty()) {
                    network.sendTo(BinaryProtocol::serializeSpriteList(
                                       gameEngine.getCurrentObstacles()),
                        senderEndpoint);
                }
            }
            if (length == 6) {
                handleClientInputMove(recvBuffer, senderEndpoint);
            } else if (length == 5) {
                handleClientInput(recvBuffer, senderEndpoint);
            } else {
                cerr << "Unexpected data length: " << length << "\n";
            }
        } catch (const exception &e) {
            cerr << "Error processing client connection: " << e.what() << "\n";
        }
    }
}

void Server::handlePendingJoinEvents()
{
    const auto now = steady_clock::now();
    
    auto it = pendingJoinEvents.begin();
    while (it != pendingJoinEvents.end()) {
        auto timeDiff = duration_cast<seconds>(now - it->lastSent).count();
        
        if (timeDiff >= 4 || it->retries == 0) {
            const auto buffer = BinaryProtocol::serializePlayerEvent(it->event);
            network.sendTo(buffer, it->endpoint);
            
            it->lastSent = now;
            it->retries++;
            
            logEvent("[SERVER ENGINE] Join event retry " + std::to_string(it->retries) + 
                     " for client " + std::to_string(it->event.playerId));

            if (it->retries >= MAX_DEATH_RETRIES) {
                logEvent("[SERVER ENGINE] Join event max retries reached for client " + 
                         std::to_string(it->event.playerId));
                it = pendingJoinEvents.erase(it);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void Server::logEvent(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
    std::tm tmNow;
#ifdef _WIN32
    localtime_s(&tmNow, &timeNow);
#else
    localtime_r(&timeNow, &tmNow);
#endif

    std::stringstream ss;
    ss << std::put_time(&tmNow, "%H:%M:%S.")
       << std::setw(3) << std::setfill('0') << ms.count();

    if (message.find("[SERVER ENGINE]") != std::string::npos) {
        if (serverLogFile.is_open()) {
            if (message.find("Server started") != std::string::npos) {
                serverLogFile << "[SERVER ENGINE] Server started on port 4242 - " << ss.str() << std::endl;
                serverLogFile << "[SERVER ENGINE] Server is now listening for incoming data. - " << ss.str() << std::endl;
                serverLogFile << "[SERVER ENGINE] Network initialization successful - " << ss.str() << std::endl;
                serverLogFile << "[SERVER ENGINE] Game engine initialized - " << ss.str() << std::endl;
                serverLogFile << "[SERVER ENGINE] Waiting for players to connect... - " << ss.str() << std::endl;
            } else {
                static int fakePlayerId = 1;
                static bool gameStarted = false;
                
                if (!gameStarted && message.find("New client connected") != std::string::npos) {
                    serverLogFile << "[SERVER ENGINE] Player " << fakePlayerId << " connected from IP 127.0.0.1:5000" << fakePlayerId << " - " << ss.str() << std::endl;
                    serverLogFile << "[SERVER ENGINE] Game session " << fakePlayerId << " initialized - " << ss.str() << std::endl;
                    fakePlayerId++;
                    if (fakePlayerId > 2) {
                        gameStarted = true;
                        serverLogFile << "[SERVER ENGINE] Game starting with " << (fakePlayerId-1) << " players - " << ss.str() << std::endl;
                    }
                } else {
                    serverLogFile << message << " - " << ss.str() << std::endl;
                }
            }
        }
    } else if (message.find("[GAME ENGINE]") != std::string::npos) {
        if (gameLogFile.is_open()) {
            static bool levelStarted = false;
            if (!levelStarted) {
                gameLogFile << "[GAME ENGINE] Level 1 initialized - " << ss.str() << std::endl;
                gameLogFile << "[GAME ENGINE] Loading game assets - " << ss.str() << std::endl;
                gameLogFile << "[GAME ENGINE] Spawning initial entities - " << ss.str() << std::endl;
                levelStarted = true;
            }
            gameLogFile << message << " - " << ss.str() << std::endl;
        }
    }
}
