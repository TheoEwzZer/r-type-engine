/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** main
*/

#include "Game/Server.hpp"
#include <iostream>

using namespace rtype;

int main()
{
    EngineConfig config;
    try {
        config.loadFromFile("config.json");
    } catch (const exception &e) {
        cerr << "Failed to load config: " << e.what() << endl;
        return 84;
    }

    Server server(config);
    server.run();
    return 0;
}
