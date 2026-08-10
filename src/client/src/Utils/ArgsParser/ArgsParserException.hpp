/*
** EPITECH PROJECT, 2024
** B-OOP-400-MAR-5-2-rtype-theo-fabiano
** Exceptions.hpp
*/

#pragma once

#include <stdexcept>

using namespace std;

class InvalidArgumentException : public exception {
public:
    explicit InvalidArgumentException(const char *const message) : msg(message)
    {
    }

    const char *what() const noexcept override { return msg; }

private:
    const char *msg;
};

class InvalidCommandLineException : public exception {
public:
    explicit InvalidCommandLineException(const char *const message) :
        msg(message)
    {
    }

    const char *what() const noexcept override { return msg; }

private:
    const char *msg;
};

class HelpException : public exception {
public:
    const char *what() const noexcept override
    {
        return "Help message displayed";
    }
};
