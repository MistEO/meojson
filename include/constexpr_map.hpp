#pragma once

#include <utility>
#include <vector>

namespace json
{

template <typename Iter, typename Val, typename Pred>
inline constexpr Iter lower_bound_fix(Iter begin, Iter end, const Val& val, Pred pred) {
#ifdef __GLIBCXX__
// lower_bound实现的有问题
// https://github.com/gcc-mirror/gcc/blob/d9375e490072d1aae73a93949aa158fcd2a27018/libstdc%2B%2B-v3/include/bits/stl_algobase.h#L1023
    return std::__lower_bound(begin, end, val, [](auto iter, const auto& val) {
        return pred(*iter, val);
    });
#else
    return std::lower_bound(begin, end, val, pred);
#endif
}

template <typename key_t, typename value_t>
class constexpr_map
{
    using container_type = std::vector<std::pair<key_t, value_t>>;

public:
    using key_type = key_t;
    using mapped_type = value_t;
    using value_type = typename container_type::value_type;

    struct iterator
    {
        typename container_type::iterator _iter;

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename container_type::iterator::value_type;
        using difference_type = typename container_type::iterator::difference_type;
        using pointer = typename container_type::iterator::pointer;
        using reference = typename container_type::iterator::reference;

        constexpr iterator(typename container_type::iterator&& it) : _iter(it) {}

        constexpr iterator& operator++()
        {
            ++_iter;
            return *this;
        }

        constexpr iterator operator++(int) { return iterator { _iter++ }; }

        constexpr iterator& operator--()
        {
            --_iter;
            return *this;
        }

        constexpr iterator operator--(int) { return iterator { _iter-- }; }

        constexpr value_type& operator*() const { return *_iter; }

        constexpr value_type* operator->() const { return &*_iter; }

        friend constexpr bool operator==(const iterator& x, const iterator& y) { return x._iter == y._iter; }

        friend constexpr bool operator!=(const iterator& x, const iterator& y) { return x._iter != y._iter; }
    };

    struct const_iterator
    {
        typename container_type::const_iterator _iter;

        using value_type = typename container_type::const_iterator::value_type;
        using difference_type = typename container_type::const_iterator::difference_type;
        using pointer = typename container_type::const_iterator::pointer;
        using reference = typename container_type::const_iterator::reference;

        constexpr const_iterator(typename container_type::const_iterator&& it) : _iter(it) {}

        constexpr const_iterator& operator++()
        {
            ++_iter;
            return *this;
        }

        constexpr const_iterator operator++(int) { return const_iterator { _iter++ }; }

        constexpr const_iterator& operator--()
        {
            --_iter;
            return *this;
        }

        constexpr const_iterator operator--(int) { return const_iterator { _iter-- }; }

        constexpr const value_type& operator*() const { return *_iter; }

        constexpr const value_type* operator->() const { return &*_iter; }

        friend constexpr bool operator==(const const_iterator& x, const const_iterator& y)
        {
            return x._iter == y._iter;
        }

        friend constexpr bool operator!=(const const_iterator& x, const const_iterator& y)
        {
            return x._iter != y._iter;
        }
    };

    constexpr constexpr_map() = default;
    constexpr constexpr_map(const constexpr_map&) = default;
    constexpr constexpr_map(constexpr_map&&) = default;
    constexpr ~constexpr_map() = default;
    constexpr constexpr_map& operator=(const constexpr_map&) = default;
    constexpr constexpr_map& operator=(constexpr_map&&) = default;

    template <typename... Args>
    constexpr constexpr_map(Args&&... args) : _data(args...)
    {}

    constexpr iterator begin() { return _data.begin(); }

    constexpr const_iterator begin() const { return _data.begin(); }

    constexpr const_iterator cbegin() const { return _data.cbegin(); }

    constexpr iterator end() { return _data.end(); }

    constexpr const_iterator end() const { return _data.end(); }

    constexpr const_iterator cend() const { return _data.cend(); }

    constexpr mapped_type& operator[](const key_t& key) { return emplace(key, mapped_type {}).first->second; }

    constexpr mapped_type& operator[](key_t&& key)
    {
        return emplace(std::forward<key_t&&>(key), mapped_type {}).first->second;
    }

    constexpr iterator find(const key_t& key)
    {
        auto ptr = lower_bound_fix(begin(), end(), key,
                                    [](const value_type& elem, const key_type& val) { return elem.first < val; });
        return ptr->first == key ? ptr : end();
    }

    constexpr const_iterator find(const key_t& key) const
    {
        auto ptr = lower_bound_fix(cbegin(), cend(), key,
                                    [](const value_type& elem, const key_type& val) { return elem.first < val; });
        return ptr->first == key ? ptr : cend();
    }

    template <typename... Args>
    constexpr std::pair<iterator, bool> emplace(Args&&... args)
    {
        value_type p(std::forward<Args&&>(args)...);
        auto ptr = lower_bound_fix(begin(), end(), p.first,
                                    [](const value_type& elem, const key_type& val) { return elem.first < val; });
        if (ptr != end() && ptr->first == p.first) {
            return std::make_pair(ptr, false);
        }
        else {
            return std::make_pair(_data.insert(ptr._iter, std::move(p)), true);
        }
    }

private:
    container_type _data;
};

} // namespace json

template <typename key_t, typename value_t>
constexpr bool operator==(const json::constexpr_map<key_t, value_t>& x, const json::constexpr_map<key_t, value_t>& y)
{
    return x._data == y._data;
}

template <typename key_t, typename value_t>
constexpr bool operator!=(const json::constexpr_map<key_t, value_t>& x, const json::constexpr_map<key_t, value_t>& y)
{
    return !(x == y);
}
