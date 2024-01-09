#pragma once

#include <utility>
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
        return this->emplace_back(key, mapped_type {}).second;
    }

    constexpr mapped_type& operator[](key_t&& key)
    {
        for (auto& data : *this) {
            if (data.first == key) {
                return data.second;
            }
        }
        return this->emplace_back(std::forward<key_t&&>(key), mapped_type {}).second;
    }

    template <typename... Args>
    constexpr std::pair<typename std::vector<std::pair<const key_t, value_t>>::iterator, bool> emplace(Args&&... args)
    {
        std::pair<const key_t, value_t> p(std::forward<Args&&>(args)...);
        for (auto it = this->begin(); it != this->end(); it++) {
            if (it->first == p.first) {
                return std::make_pair(it, false);
            }
        }
        this->push_back(std::move(p));
        return std::make_pair(this->end() - 1, true);
    }
};
}
