#pragma once

#include <utility>
#include <vector>

namespace json
{

template <typename key_t, typename value_t>
class constexpr_map
{
    using container_type = std::vector<std::pair<const key_t, value_t>>;
public:
    using key_type = const key_t;
    using mapped_type = value_t;
    using value_type = typename container_type::value_type;

    struct iterator {
        typename container_type::iterator _iter;

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename container_type::iterator::value_type;
        using difference_type = typename container_type::iterator::difference_type;
        using pointer = typename container_type::iterator::pointer;
        using reference = typename container_type::iterator::reference;

        constexpr iterator(typename container_type::iterator&& it) : _iter(it) {}

        constexpr iterator& operator++() {
            ++_iter;
            return *this;
        }

        constexpr iterator operator++(int) {
            return iterator { _iter++ };
        }

        constexpr iterator& operator--() {
            --_iter;
            return *this;
        }

        constexpr iterator operator--(int) {
            return iterator { _iter-- };
        }

        constexpr value_type& operator*() const {
            return *_iter;
        }

        constexpr value_type* operator->() const {
            return &*_iter;
        }

        friend constexpr bool operator==(const iterator& x, const iterator& y) {
            return x._iter == y._iter;
        }

        friend constexpr bool operator!=(const iterator& x, const iterator& y) {
            return x._iter != y._iter;
        }
    };

    struct const_iterator {
        typename container_type::const_iterator _iter;

        using value_type = typename container_type::const_iterator::value_type;
        using difference_type = typename container_type::const_iterator::difference_type;
        using pointer = typename container_type::const_iterator::pointer;
        using reference = typename container_type::const_iterator::reference;

        constexpr const_iterator(typename container_type::const_iterator&& it) : _iter(it) {}

        constexpr const_iterator& operator++() {
            ++_iter;
            return *this;
        }

        constexpr const_iterator operator++(int) {
            return const_iterator { _iter++ };
        }

        constexpr const_iterator& operator--() {
            --_iter;
            return *this;
        }

        constexpr const_iterator operator--(int) {
            return const_iterator { _iter-- };
        }

        constexpr const value_type& operator*() const {
            return *_iter;
        }

        constexpr const value_type* operator->() const {
            return &*_iter;
        }

        friend constexpr bool operator==(const const_iterator& x, const const_iterator& y) {
            return x._iter == y._iter;
        }
        
        friend constexpr bool operator!=(const const_iterator& x, const const_iterator& y) {
            return x._iter != y._iter;
        }
    };

    constexpr constexpr_map() = default;
    constexpr constexpr_map(const constexpr_map& ) = default;
    constexpr constexpr_map(constexpr_map&& ) = default;
    constexpr ~constexpr_map() = default;
    constexpr constexpr_map& operator=(const constexpr_map& ) = default;
    constexpr constexpr_map& operator=(constexpr_map&& ) = default;

    template <typename... Args>
    constexpr constexpr_map(Args&&... args) : _data(args...)
    {}

    constexpr iterator begin() {
        return _data.begin();
    }

    constexpr const_iterator begin() const {
        return _data.begin();
    }

    constexpr const_iterator cbegin() const {
        return _data.cbegin();
    }

    constexpr iterator end() {
        return _data.end();
    }

    constexpr const_iterator end() const {
        return _data.end();
    }

    constexpr const_iterator cend() const {
        return _data.cend();
    }

    constexpr mapped_type& operator[](const key_t& key)
    {
        for (auto& data : _data) {
            if (data.first == key) {
                return data.second;
            }
        }
        return _data.emplace_back(key, mapped_type {}).second;
    }

    constexpr mapped_type& operator[](key_t&& key)
    {
        for (auto& data : _data) {
            if (data.first == key) {
                return data.second;
            }
        }
        return _data.emplace_back(std::forward<key_t&&>(key), mapped_type {}).second;
    }

    constexpr iterator find(const key_t& key) {
        return std::find_if(begin(), end(), [&key](const auto& v) { return v.first == key; });
    }

    constexpr const_iterator find(const key_t& key) const {
        return std::find_if(begin(), end(), [&key](const auto& v) { return v.first == key; });
    }

    template <typename... Args>
    constexpr std::pair<iterator, bool> emplace(Args&&... args)
    {
        std::pair<const key_t, value_t> p(std::forward<Args&&>(args)...);
        for (auto it = begin(); it != end(); it++) {
            if (it->first == p.first) {
                return std::make_pair(it, false);
            }
        }
        _data.push_back(std::move(p));
        return std::make_pair(--end(), true);
    }
private:
    container_type _data;
};

}

template <typename key_t, typename value_t>
constexpr bool operator==(const json::constexpr_map<key_t, value_t>& x, const json::constexpr_map<key_t, value_t>& y)
{
    if (x.size() != y.size()) {
        return false;
    }
    for (const auto& [key, val] : x) {
        bool found = false;
        for (const auto& [key2, val2] : y) {
            if (key == key2) {
                if (!(val == val2)) {
                    return false;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

template <typename key_t, typename value_t>
constexpr bool operator!=(const json::constexpr_map<key_t, value_t>& x, const json::constexpr_map<key_t, value_t>& y)
{
    return !(x == y);
}
