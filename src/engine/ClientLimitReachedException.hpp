/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** ClientLimitReachedException
*/

#include <exception>
#include <string>

using namespace std;

namespace rtype {
class ClientLimitReachedException : public exception {
public:
    explicit ClientLimitReachedException(const string &message) :
        message(message)
    {
    }

    const char *what() const noexcept override { return message.c_str(); }

private:
    string message;
};
}
