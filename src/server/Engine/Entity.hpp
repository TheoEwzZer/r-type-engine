/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Entity
*/

#pragma once

#include <cstddef>
#include <functional>

namespace rtype::ecs {
class Entity {
public:
    explicit operator unsigned int() const { return id; }

    [[nodiscard]] unsigned short getIdShort() const
    {
        return static_cast<unsigned short>(id);
    }

    [[nodiscard]] static Entity create(const unsigned int id)
    {
        return Entity(id);
    }

    bool operator==(const Entity &other) const = default;

private:
    explicit Entity(const unsigned int id) : id(id) {}

    const unsigned int id;

    friend class Registry;
};
}
