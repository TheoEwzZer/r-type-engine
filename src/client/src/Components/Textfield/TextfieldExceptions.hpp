#include <exception>
#include <string>

class TextfieldExceptions : public exception {
public:
    explicit TextfieldExceptions(const string &message) : _message(message) {}

    const char *what() const noexcept override { return _message.c_str(); }

private:
    string _message;
};
