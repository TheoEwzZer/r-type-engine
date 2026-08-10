/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Colorblind
*/

#pragma once
#include <exception>
#include <string>

using namespace std;

class ColorblindFilterException : public exception {
public:
    explicit ColorblindFilterException(const string &message) :
        message(message)
    {
    }

    const char *what() const noexcept override { return message.c_str(); }

private:
    string message;
};
