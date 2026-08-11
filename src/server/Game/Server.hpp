/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Server
*/
#pragma once

#include "AssetManager.hpp"
#include "Components/AI.hpp"
#include "Components/Animation.hpp"
#include "Components/Controllable.hpp"
#include "Components/Player.hpp"
#include "Components/Position.hpp"
#include "Components/Projectile.hpp"
#include "EngineConfig.hpp"
#include "GameEngine.hpp"
#include "Network.hpp"
#include "Protocol.hpp"
#include "Registry.hpp"
#include <asio.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;
using namespace asio;
using namespace asio::ip;
using namespace rtype::ecs;
using enum rtype::GameplayAsset;

namespace rtype {

class ThreadPool {
public:
    ThreadPool() = default;
    vector<jthread> threads;
    queue<function<void()>> tasks;
    mutex queueMutex;
    condition_variable condition;
    bool stop = false;
};

class Server {
public:
    explicit Server(const EngineConfig &config);
    ~Server();
    [[noreturn]] void run();

private:
    void startReceive();
    void handleReceivedData(const error_code &error, size_t bytesTransferred,
        udp::endpoint senderEndpoint);
    void handleClientInput(const vector<unsigned char> &data,
        const udp::endpoint &clientEndpoint);
    void handleClientInputMove(const vector<unsigned char> &data,
        const udp::endpoint &clientEndpoint);
    void sendEntityStates();
    void processClientConnections();
    void networkThreadFunction();
    void gameThreadFunction();
    void enqueueNetworkTask(function<void()> task);
    void enqueueGameTask(function<void()> task);
    bool getNetworkTask(function<void()> &task);
    bool getGameTask(function<void()> &task);

    io_context ioContext;
    Network network;
    Registry registry;
    vector<unsigned char> recvBuffer;
    steady_clock::time_point lastUpdate;
    EngineConfig config;
    GameEngine gameEngine;
    bool isGameStarted = false;
    std::map<unsigned int, Sprite> previousSprites;
    int tickCounter = 0;

    vector<jthread> threadPool;
    asio::strand<asio::io_context::executor_type> strand;
    const size_t networkThreads = 4;
    const size_t gameThreads = 2;

    ThreadPool networkPool;
    ThreadPool gamePool;
    mutex stateMutex;
    atomic<bool> processingGameState { false };

    ::std::ofstream logFile;
    std::ofstream serverLogFile;
    std::ofstream gameLogFile;
    std::map<unsigned int, std::ofstream> playerLogFiles; // Ajouté
    void logEvent(const ::string &message);

    void initThreadPools();
    void processGameLogic();
    void broadcastStates();

    std::mutex logMutex;

    struct DeathEventRetry {
        PlayerEvent event;
        steady_clock::time_point lastSent;
        int retries;
    };

    vector<DeathEventRetry> pendingDeathEvents;
    const int MAX_DEATH_RETRIES = 10;
    void handlePendingDeathEvents();

    struct EventRetry {
        PlayerEvent event;
        steady_clock::time_point lastSent;
        int retries;
        udp::endpoint endpoint;
    };

    vector<EventRetry> pendingJoinEvents;
    void handlePendingJoinEvents();

    vector<EventRetry> pendingForceEvents;
    void handlePendingForceEvents();

    uint32_t nextPacketId = 1;
    void handleAck(const vector<unsigned char> &data);
};
}
