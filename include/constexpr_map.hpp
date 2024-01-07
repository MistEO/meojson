#pragma once

#include <vector>

namespace json
{
template <typename key_t, typename value_t>
class constexpr_map : public std::vector<std::pair<const key_t, value_t>>
{
public:
    using key_type = const key_t;
    using mapped_type = value_t;

    template <typename... Args>
    constexpr constexpr_map(Args&&... args) : std::vector<std::pair<const key_t, value_t>>(args...)
    {}

    constexpr mapped_type& operator[](const key_t& key)
    {
        for (auto& data : *this) {
            if (data.first == key) {
                return data.second;
            }
        }
        this->emplace_back(key, mapped_type {});
        return this->back().second;
    }

    constexpr mapped_type& operator[](key_t&& key)
    {
        for (auto& data : *this) {
            if (data.first == key) {
                return data.second;
            }
        }
        this->emplace_back(key, mapped_type {});
        return this->back().second;
    }

    template <typename... Args>
    constexpr std::pair<const key_t, value_t> emplace(Args&&... args)
    {
        std::pair<const key_t, value_t> p(args...);
        this->operator[](p.first) = p.second;
        return p;
    }
};
}
