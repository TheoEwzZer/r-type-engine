/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** ArgsParser
*/

#include "ArgsParser.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>

void ArgsParser::parseArgs(const int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--skin" && i + 1 < argc) {
            int skinValue = stoi(argv[++i]);
            if (skinValue < 1 || skinValue > 4) {
                throw InvalidArgumentException(
                    "Invalid skin value. Choose between 1, 2, 3, or 4.");
            }
            skin = skinValue;
        }
    }
}

int ArgsParser::getSkin() const { return skin; }

bool ArgsParser::isValidIPv4(const string &ip) const
{
    istringstream stream(ip);
    string octet;
    int count = 0;

    while (getline(stream, octet, '.')) {
        count++;
        if (count > 4) {
            return false;
        }

        if (octet.empty() || !ranges::all_of(octet, ::isdigit)) {
            return false;
        }

        int value = stoi(octet);
        if (value < 0 || value > 255) {
            return false;
        }
    }
    return count == 4;
}
