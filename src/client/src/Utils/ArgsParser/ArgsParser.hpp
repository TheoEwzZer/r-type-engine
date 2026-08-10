/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** ArgsParser.hpp
*/

#pragma once

#include "ArgsParserException.hpp"
#include <memory>

using namespace std;

class ArgsParser {
public:
    enum class LaunchMode { CLIENT, EDITOR };

    ArgsParser() : skin(0) {}

    ~ArgsParser() = default;

    void parseArgs(const int argc, char **argv);
    int getSkin() const;

private:
    bool isValidIPv4(const string &ip) const;
    int skin;
};
