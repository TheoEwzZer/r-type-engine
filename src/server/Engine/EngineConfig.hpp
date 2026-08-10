/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** EngineConfig
*/

#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace rtype {
class EngineConfig {
public:
    EngineConfig() = default;

    bool enableObstacles = true;
    bool enableAnimation = true;
    bool enableAI = true;

    void loadFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Could not open config file");
        }
        nlohmann::json configJson;
        file >> configJson;
        enableObstacles = configJson.value("enableObstacles", true);
        enableAnimation = configJson.value("enableAnimation", true);
        enableAI = configJson.value("enableAI", false);
    }
};
}
