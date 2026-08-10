#pragma once

#include "../Component.hpp"

namespace rtype {

struct Score : public Component {
    Score(unsigned int value = 0) : value(value) {}

    unsigned int value;
    static unsigned int globalScore;
    static unsigned int bestScore;
};

}
