#pragma once

#include "object.hpp"

namespace json
{
inline object::object() = default;
inline object::object(const object& rhs) = default;
inline object::object(object&& rhs) noexcept = default;
inline object::~object() = default;
inline object& object::operator=(const object&) = default;
inline object& object::operator=(object&&) noexcept = default;

inline object::object(std::initializer_list<value_type> init_list)
    : _object_data(std::make_move_iterator(init_list.begin()), std::make_move_iterator(init_list.end()))
{
}

inline bool object::contains(const std::string& key) const
{
    return _object_data.find(key) != _object_data.cend();
}

inline bool object::empty() const noexcept
{
    return _object_data.empty();
}

inline size_t object::size() const noexcept
{
    return _object_data.size();
}

inline bool object::exists(const std::string& key) const
{
    return contains(key);
}

inline const value& object::at(const std::string& key) const
{
    return _object_data.at(key);
}

inline void object::clear() noexcept
{
    _object_data.clear();
}

inline bool object::erase(const std::string& key)
{
    return _object_data.erase(key) > 0 ? true : false;
}

inline bool object::erase(iterator iter)
{
    return _object_data.erase(iter) != _object_data.end();
}

template <typename... args_t>
inline decltype(auto) object::emplace(args_t&&... args)
{
    static_assert(std::is_constructible_v<value_type, args_t...>, "Parameter can't be used to construct a raw_object::value_type");
    return _object_data.insert_or_assign(std::forward<args_t>(args)...);
}

template <typename... args_t>
inline decltype(auto) object::insert(args_t&&... args)
{
    return emplace(std::forward<args_t>(args)...);
}

inline std::string object::to_string() const
{
    std::string str { '{' };
    for (auto iter = _object_data.cbegin(); iter != _object_data.cend();) {
        const auto& [key, val] = *iter;
        str += '"' + _utils::unescape_string(key) + std::string { '\"', ':' } + val.to_string();
        if (++iter != _object_data.cend()) {
            str += ',';
        }
    }
    str += '}';
    return str;
}

inline std::string object::format(size_t indent, size_t indent_times) const
{
    const std::string tail_indent(indent * indent_times, ' ');
    const std::string body_indent(indent * (indent_times + 1), ' ');

    std::string str { '{', '\n' };
    for (auto iter = _object_data.cbegin(); iter != _object_data.cend();) {
        const auto& [key, val] = *iter;
        str += body_indent + '"' + _utils::unescape_string(key) + std::string { '\"', ':', ' ' } + val.format(indent, indent_times + 1);
        if (++iter != _object_data.cend()) {
            str += ',';
        }
        str += '\n';
    }
    str += tail_indent + '}';
    return str;
}

inline std::string object::dumps(std::optional<size_t> indent) const
{
    return indent ? format(*indent) : to_string();
}

inline std::string object::format(size_t indent) const
{
    return format(indent, 0);
}

template <typename value_t>
inline bool object::all() const
{
    for (const auto& [_, val] : _object_data) {
        if (!val.template is<value_t>()) {
            return false;
        }
    }
    return true;
}

template <typename... key_then_default_value_t>
inline auto object::get(key_then_default_value_t&&... keys_then_default_value) const
{
    return get(std::forward_as_tuple(keys_then_default_value...), std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
}

template <typename... key_then_default_value_t, size_t... keys_indexes_t>
inline auto object::get(std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const
{
    constexpr unsigned long default_value_index = sizeof...(key_then_default_value_t) - 1;
    return get_helper(std::get<default_value_index>(keys_then_default_value), std::get<keys_indexes_t>(keys_then_default_value)...);
}

template <typename value_t, typename... rest_keys_t>
inline auto object::get_helper(const value_t& default_value, const std::string& key, rest_keys_t&&... rest) const
{
    constexpr bool is_json = std::is_same_v<value, value_t> || std::is_same_v<array, value_t> || std::is_same_v<object, value_t>;
    constexpr bool is_string = std::is_constructible_v<std::string, value_t> && !is_json;

    if (!contains(key)) {
        if constexpr (is_string) {
            return std::string(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    return at(key).get_helper(default_value, std::forward<rest_keys_t>(rest)...);
}

template <typename value_t>
inline auto object::get_helper(const value_t& default_value, const std::string& key) const
{
    constexpr bool is_json = std::is_same_v<value, value_t> || std::is_same_v<array, value_t> || std::is_same_v<object, value_t>;
    constexpr bool is_string = std::is_constructible_v<std::string, value_t> && !is_json;

    if (!contains(key)) {
        if constexpr (is_string) {
            return std::string(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    auto val = _object_data.at(key);
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
inline std::optional<value_t> object::find(const std::string& key) const
{
    auto iter = _object_data.find(key);
    if (iter == _object_data.end()) {
        return std::nullopt;
    }
    const auto& val = iter->second;
    return val.template is<value_t>() ? std::optional<value_t>(val.template as<value_t>()) : std::nullopt;
}

inline typename object::iterator object::begin() noexcept
{
    return _object_data.begin();
}

inline typename object::iterator object::end() noexcept
{
    return _object_data.end();
}

inline typename object::const_iterator object::begin() const noexcept
{
    return _object_data.begin();
}

inline typename object::const_iterator object::end() const noexcept
{
    return _object_data.end();
}

inline typename object::const_iterator object::cbegin() const noexcept
{
    return _object_data.cbegin();
}

inline typename object::const_iterator object::cend() const noexcept
{
    return _object_data.cend();
}

inline value& object::operator[](const std::string& key)
{
    return _object_data[key];
}

inline value& object::operator[](std::string&& key)
{
    return _object_data[std::move(key)];
}

inline object object::operator|(const object& rhs) const&
{
    object temp = *this;
    for (const auto& [key, val] : rhs) {
        temp._object_data[key] = val;
    }
    return temp;
}

inline object object::operator|(object&& rhs) const&
{
    object temp = *this;
    for (auto& [key, val] : rhs) {
        temp._object_data[key] = std::move(val);
    }
    return temp;
}

inline object object::operator|(const object& rhs) &&
{
    for (const auto& [key, val] : rhs) {
        _object_data[key] = val;
    }
    return std::move(*this);
}

inline object object::operator|(object&& rhs) &&
{
    for (auto& [key, val] : rhs) {
        _object_data[key] = std::move(val);
    }
    return std::move(*this);
}

inline object& object::operator|=(const object& rhs)
{
    for (const auto& [key, val] : rhs) {
        _object_data[key] = val;
    }
    return *this;
}

inline object& object::operator|=(object&& rhs)
{
    for (auto& [key, val] : rhs) {
        _object_data[key] = std::move(val);
    }
    return *this;
}

inline bool object::operator==(const object& rhs) const
{
    return _object_data == rhs._object_data;
}

inline bool object::operator!=(const object& rhs) const
{
    return !(*this == rhs);
}

inline std::ostream& operator<<(std::ostream& out, const object& obj)
{
    out << obj.format();
    return out;
}

template <typename T>
inline T object::as() const&
{
    if constexpr (_utils::is_map<T> && std::is_same_v<typename T::key_type, std::string>) {
        T result;
        for (const auto& [key, val] : _object_data) {
            result.emplace(key, val.as<typename T::mapped_type>());
        }
        return result;
    }
    else {
        static_assert(!sizeof(T), "Unsupported type for object::as()");
    }
}

template <typename T>
inline T object::as() &&
{
    if constexpr (_utils::is_map<T> && std::is_same_v<typename T::key_type, std::string>) {
        T result;
        for (auto& [key, val] : _object_data) {
            result.emplace(key, std::move(val).as<typename T::mapped_type>());
        }
        return result;
    }
    else {
        static_assert(!sizeof(T), "Unsupported type for object::as()");
    }
}
} // namespace json
