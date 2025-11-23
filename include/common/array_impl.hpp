#pragma once

#include "array.hpp"

namespace json
{
inline array::array(std::initializer_list<value_type> init_list)
    : _array_data(init_list)
{
}

inline array::array(typename raw_array::size_type size)
    : _array_data(size)
{
}

inline bool array::empty() const noexcept
{
    return _array_data.empty();
}

inline size_t array::size() const noexcept
{
    return _array_data.size();
}

inline bool array::contains(size_t pos) const
{
    return pos < _array_data.size();
}

inline bool array::exists(size_t pos) const
{
    return contains(pos);
}

inline void array::clear() noexcept
{
    _array_data.clear();
}

inline bool array::erase(size_t pos)
{
    return erase(_array_data.begin() + pos);
}

inline bool array::erase(iterator iter)
{
    return _array_data.erase(iter) != _array_data.end();
}

template <typename... args_t>
inline decltype(auto) array::emplace_back(args_t&&... args)
{
    static_assert(std::is_constructible_v<value_type, args_t...>, "Parameter can't be used to construct a raw_array::value_type");
    return _array_data.emplace_back(std::forward<args_t>(args)...);
}

template <typename... args_t>
inline decltype(auto) array::push_back(args_t&&... args)
{
    return emplace_back(std::forward<args_t>(args)...);
}

inline const value& array::at(size_t pos) const
{
    return _array_data.at(pos);
}

inline std::string array::to_string() const
{
    std::string str { '[' };
    for (auto iter = _array_data.cbegin(); iter != _array_data.cend();) {
        str += iter->to_string();
        if (++iter != _array_data.cend()) {
            str += ',';
        }
    }
    str += ']';
    return str;
}

inline std::string array::format(size_t indent, size_t indent_times) const
{
    const std::string tail_indent(indent * indent_times, ' ');
    const std::string body_indent(indent * (indent_times + 1), ' ');

    std::string str { '[', '\n' };
    for (auto iter = _array_data.cbegin(); iter != _array_data.cend();) {
        str += body_indent + iter->format(indent, indent_times + 1);
        if (++iter != _array_data.cend()) {
            str += ',';
        }
        str += '\n';
    }
    str += tail_indent + ']';
    return str;
}

inline std::string array::dumps(std::optional<size_t> indent) const
{
    return indent ? format(*indent) : to_string();
}

inline std::string array::format(size_t indent) const
{
    return format(indent, 0);
}

template <typename value_t>
inline bool array::all() const
{
    for (const auto& elem : _array_data) {
        if (!elem.template is<value_t>()) {
            return false;
        }
    }
    return true;
}

template <typename... key_then_default_value_t>
inline auto array::get(key_then_default_value_t&&... keys_then_default_value) const
{
    return get(std::forward_as_tuple(keys_then_default_value...), std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
}

template <typename... key_then_default_value_t, size_t... keys_indexes_t>
inline auto array::get(std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const
{
    constexpr unsigned long default_value_index = sizeof...(key_then_default_value_t) - 1;
    return get_helper(std::get<default_value_index>(keys_then_default_value), std::get<keys_indexes_t>(keys_then_default_value)...);
}

template <typename value_t, typename... rest_keys_t>
inline auto array::get_helper(const value_t& default_value, size_t pos, rest_keys_t&&... rest) const
{
    constexpr bool is_json = std::is_same_v<value, value_t> || std::is_same_v<array, value_t> || std::is_same_v<object, value_t>;
    constexpr bool is_string = std::is_constructible_v<std::string, value_t> && !is_json;

    if (!contains(pos)) {
        if constexpr (is_string) {
            return std::string(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    return at(pos).get_helper(default_value, std::forward<rest_keys_t>(rest)...);
}

template <typename value_t>
inline auto array::get_helper(const value_t& default_value, size_t pos) const
{
    constexpr bool is_json = std::is_same_v<value, value_t> || std::is_same_v<array, value_t> || std::is_same_v<object, value_t>;
    constexpr bool is_string = std::is_constructible_v<std::string, value_t> && !is_json;

    if (!contains(pos)) {
        if constexpr (is_string) {
            return std::string(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    auto val = _array_data.at(pos);
    if (val.template is<value_t>()) {
        if constexpr (is_string) {
            return val.template as<std::string>();
        }
        else {
            return val.template as<value_t>();
        }
    }
    else {
        if constexpr (is_string) {
            return std::string(default_value);
        }
        else {
            return value_t(default_value);
        }
    }
}

template <typename value_t>
inline std::optional<value_t> array::find(size_t pos) const
{
    if (!contains(pos)) {
        return std::nullopt;
    }
    const auto& val = _array_data.at(pos);
    return val.template is<value_t>() ? std::optional<value_t>(val.template as<value_t>()) : std::nullopt;
}

inline typename array::iterator array::begin() noexcept
{
    return _array_data.begin();
}

inline typename array::iterator array::end() noexcept
{
    return _array_data.end();
}

inline typename array::const_iterator array::begin() const noexcept
{
    return _array_data.begin();
}

inline typename array::const_iterator array::end() const noexcept
{
    return _array_data.end();
}

inline typename array::const_iterator array::cbegin() const noexcept
{
    return _array_data.cbegin();
}

inline typename array::const_iterator array::cend() const noexcept
{
    return _array_data.cend();
}

inline typename array::reverse_iterator array::rbegin() noexcept
{
    return _array_data.rbegin();
}

inline typename array::reverse_iterator array::rend() noexcept
{
    return _array_data.rend();
}

inline typename array::const_reverse_iterator array::rbegin() const noexcept
{
    return _array_data.rbegin();
}

inline typename array::const_reverse_iterator array::rend() const noexcept
{
    return _array_data.rend();
}

inline typename array::const_reverse_iterator array::crbegin() const noexcept
{
    return _array_data.crbegin();
}

inline typename array::const_reverse_iterator array::crend() const noexcept
{
    return _array_data.crend();
}

inline value& array::operator[](size_t pos)
{
    return _array_data[pos];
}

inline const value& array::operator[](size_t pos) const
{
    return _array_data[pos];
}

inline array array::operator+(const array& rhs) const&
{
    array temp = *this;
    temp._array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
    return temp;
}

inline array array::operator+(array&& rhs) const&
{
    array temp = *this;
    temp._array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return temp;
}

inline array array::operator+(const array& rhs) &&
{
    _array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
    return std::move(*this);
}

inline array array::operator+(array&& rhs) &&
{
    _array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return std::move(*this);
}

inline array& array::operator+=(const array& rhs)
{
    _array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
    return *this;
}

inline array& array::operator+=(array&& rhs)
{
    _array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return *this;
}

inline bool array::operator==(const array& rhs) const
{
    return _array_data == rhs._array_data;
}

inline bool array::operator!=(const array& rhs) const
{
    return !(*this == rhs);
}

inline std::ostream& operator<<(std::ostream& out, const array& arr)
{
    out << arr.format();
    return out;
}
} // namespace json