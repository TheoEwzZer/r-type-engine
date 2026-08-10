/*
** EPITECH PROJECT, 2024
** r-type
** File description:
** MainMenu.cpp
*/

#pragma once

#include <exception>
#include <string>

class MainMenuException : public exception {
public:
    explicit MainMenuException(const string &message) : _message(message) {}

    const char *what() const noexcept override { return _message.c_str(); }

private:
    string _message;
};
