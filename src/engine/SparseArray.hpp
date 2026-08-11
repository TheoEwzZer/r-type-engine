/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** SparseArray
*/

#pragma once

#include <algorithm>
#include <optional>
#include <vector>

using namespace std;

namespace rtype::ecs {
template <typename Component> class SparseArray {
public:
    using value_type = optional<Component>;
    using reference_type = value_type &;
    using const_reference_type = const value_type &;
    using container_t = vector<value_type>;
    using size_type = typename container_t::size_type;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;

    SparseArray() = default;
    SparseArray(const SparseArray &) = default;
    SparseArray(SparseArray &&) noexcept = default;
    ~SparseArray() = default;

    SparseArray &operator=(const SparseArray &) = default;
    SparseArray &operator=(SparseArray &&) noexcept = default;

    reference_type operator[](const size_t idx)
    {
        if (idx >= data.size()) {
            data.resize(idx + 1);
        }
        return data[idx];
    }

    const_reference_type operator[](const size_t idx) const
    {
        static const value_type empty {};
        return (idx < data.size()) ? data[idx] : empty;
    }

    void reserve(size_type new_cap) { data.reserve(new_cap); }

    iterator begin() { return data.begin(); }

    const_iterator begin() const { return data.begin(); }

    const_iterator cbegin() const { return data.cbegin(); }

    iterator end() { return data.end(); }

    const_iterator end() const { return data.end(); }

    const_iterator cend() const { return data.cend(); }

    size_type size() const { return data.size(); }

    reference_type insertAt(size_type pos, const Component &component)
    {
        if (pos >= data.size()) {
            data.resize(pos + 1);
        }
        data[pos] = component;
        return data[pos];
    }

    reference_type insertAt(size_type pos, Component &&component)
    {
        if (pos >= data.size()) {
            data.resize(pos + 1);
        }
        data[pos] = move(component);
        return data[pos];
    }

    template <class... Params>
    reference_type emplaceAt(size_type pos, Params &&...params)
    {
        using Alloc = allocator<Component>;
        static Alloc alloc;
        if (pos >= data.size()) {
            data.resize(pos + 1);
        }
        if (data[pos].has_value()) {
            allocator_traits<Alloc>::destroy(alloc, addressof(*data[pos]));
        }
        data[pos].emplace(forward<Params>(params)...);
        return data[pos];
    }

    void erase(size_type pos)
    {
        if (pos < data.size()) {
            data[pos] = nullopt;
        }
    }

    size_type getIndex(const value_type &component) const
    {
        auto it = ranges::find(data, component);
        return it != data.end() ? distance(data.begin(), it) : data.size();
    }

    void compact()
    {
        while ((!data.empty()) && (!data.back().has_value())) {
            data.pop_back();
        }
    }

private:
    container_t data;
};
}
