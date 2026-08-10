/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** Registry
*/

#pragma once

#include "ClientLimitReachedException.hpp"
#include "Entity.hpp"
#include "SparseArray.hpp"
#include <any>
#include <functional>
#include <iostream>
#include <stack>
#include <typeindex>
#include <unordered_map>

using namespace std;

namespace rtype::ecs {

class Registry {
public:
    static constexpr unsigned int RESERVED_CLIENT_IDS = 4;

    template <typename Component>
    vector<pair<Entity, Component &>> getEntities()
    {
        vector<pair<Entity, Component &>> entitiesWithComponent;
        auto &components = getComponents<Component>();
        const size_t componentCount = components.size();
        for (size_t i = 0; i < componentCount; ++i) {
            if (components[i].has_value()) {
                entitiesWithComponent.emplace_back(
                    Entity(static_cast<unsigned int>(i)),
                    components[i].value());
            }
        }
        return entitiesWithComponent;
    }

    Entity getEntity(const unsigned int idx) const { return Entity(idx); }

    template <class Component> SparseArray<Component> &registerComponent()
    {
        const auto typeIndex = type_index { typeid(Component) };
        if (!componentsArrays.contains(typeIndex)) {
            auto sparseArr = make_any<SparseArray<Component>>();
            componentsArrays[typeIndex] = sparseArr;

            componentDestroyers[typeIndex] = [](Registry &r, const Entity e) {
                r.getComponents<Component>().erase(
                    static_cast<unsigned int>(e));
            };
            componentCompactors[typeIndex]
                = [](Registry &r) { r.getComponents<Component>().compact(); };
        }
        return any_cast<SparseArray<Component> &>(componentsArrays[typeIndex]);
    }

    template <class Component> SparseArray<Component> &getComponents()
    {
        const auto typeIndex = type_index { typeid(Component) };
        return any_cast<SparseArray<Component> &>(
            componentsArrays.at(typeIndex));
    }

    template <class Component>
    const SparseArray<Component> &getComponents() const
    {
        const auto typeIndex = type_index { typeid(Component) };
        return any_cast<const SparseArray<Component> &>(
            componentsArrays.at(typeIndex));
    }

    template <class Component>
    typename SparseArray<Component>::reference_type getComponent(
        const Entity &entity)
    {
        return getComponents<Component>()[static_cast<unsigned int>(entity)];
    }

    Entity spawnEntity()
    {
        if (!freeEntities.empty()) {
            const unsigned int idx = freeEntities.top();
            freeEntities.pop();
            entitiesAlive[idx] = true;
            return Entity(idx);
        } else {
            entitiesAlive.emplace_back(true);
            return Entity(static_cast<unsigned int>(entitiesAlive.size() - 1));
        }
    }

    Entity spawnClientEntity()
    {
        for (unsigned int i = 0; i < RESERVED_CLIENT_IDS; ++i) {
            if (i >= entitiesAlive.size()) {
                entitiesAlive.emplace_back(true);
                return Entity(i);
            } else if (!entitiesAlive[i]) {
                entitiesAlive[i] = true;
                return Entity(i);
            }
        }
        throw ClientLimitReachedException(
            "Maximum number of clients reached.");
    }

    void killEntity(const Entity &e)
    {
        const auto idx = static_cast<unsigned int>(e);
        if ((idx < entitiesAlive.size()) && entitiesAlive[idx]) {
            entitiesAlive[idx] = false;
            freeEntities.push(idx);
            for (const auto &[key, destroyer] : componentDestroyers) {
                destroyer(*this, e);
            }
            for (const auto &[key, compactor] : componentCompactors) {
                compactor(*this);
            }
        }
    }

    void killEntity(const unsigned int idx)
    {
        if ((idx < entitiesAlive.size()) && entitiesAlive[idx]) {
            entitiesAlive[idx] = false;
            freeEntities.push(idx);
            for (const auto &[key, destroyer] : componentDestroyers) {
                destroyer(*this, Entity(idx));
            }
            for (const auto &[key, compactor] : componentCompactors) {
                compactor(*this);
            }
        }
    }

    template <typename Component>
    typename SparseArray<Component>::reference_type addComponent(
        const Entity &to, Component &&c)
    {
        return getComponents<Component>().insertAt(
            static_cast<unsigned int>(to), forward<Component>(c));
    }

    template <typename Component, typename... Params>
    typename SparseArray<Component>::reference_type emplaceComponent(
        const Entity &to, Params &&...p)
    {
        return getComponents<Component>().emplaceAt(
            static_cast<unsigned int>(to), forward<Params>(p)...);
    }

    template <typename Component> void removeComponent(const Entity &from)
    {
        auto &components = getComponents<Component>();
        components.erase(static_cast<unsigned int>(from));
    }

    template <class... Components, typename Function>
    void addSystem(Function &&func)
    {
        systems.push_back([func = forward<Function>(func)](Registry &r) {
            func(r, r.getComponents<Components>()...);
        });
    }

    void runSystems()
    {
        for (const auto &sys : systems) {
            sys(*this);
        }
    }

    bool isEntityAlive(const Entity &e) const
    {
        return entitiesAlive[static_cast<unsigned int>(e)];
    }

private:
    unordered_map<type_index, any> componentsArrays;
    unordered_map<type_index, function<void(Registry &, Entity)>>
        componentDestroyers;
    vector<bool> entitiesAlive = vector<bool>(RESERVED_CLIENT_IDS);
    stack<unsigned int> freeEntities;
    vector<function<void(Registry &)>> systems;
    unordered_map<type_index, function<void(Registry &)>> componentCompactors;
};
}
