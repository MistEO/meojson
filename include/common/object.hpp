// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <initializer_list>
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>

#include "exception.hpp"
#include "utils.hpp"

namespace json
{
class object
{
    friend class value;
    friend class array;

public:
    using raw_object = std::map<std::string, value>;
    using key_type = typename raw_object::key_type;
    using mapped_type = typename raw_object::mapped_type;
    using value_type = typename raw_object::value_type;
    using iterator = typename raw_object::iterator;
    using const_iterator = typename raw_object::const_iterator;

public:
    object();
    object(const object& rhs);
    object(object&& rhs) noexcept;
    object(std::initializer_list<value_type> init_list);

    // explicit object(const value& val);
    // explicit object(value&& val);

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, object> && _utils::has_to_json_in_member<jsonization_t>::value,
            bool> = true>
    object(const jsonization_t& val)
        : object(val.to_json())
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, object> && _utils::has_to_json_in_templ_spec<std::decay_t<jsonization_t>>::value,
            bool> = true>
    object(const jsonization_t& val)
        : object(ext::jsonization<std::decay_t<jsonization_t>>().to_json(val))
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, object> && std::is_rvalue_reference_v<jsonization_t&&>
                && _utils::has_move_to_json_in_templ_spec<std::decay_t<jsonization_t>>::value,
            bool> = true>
    object(jsonization_t&& val)
        : object(ext::jsonization<std::decay_t<jsonization_t>>().move_to_json(std::move(val)))
    {
    }

    // Native support for maps (std::map<std::string, T>, std::unordered_map<std::string, T>)
    template <
        typename map_t,
        std::enable_if_t<
            _utils::is_map<map_t> && std::is_same_v<typename map_t::key_type, std::string>
                && !std::is_same_v<std::decay_t<map_t>, object> && !_utils::has_to_json_in_member<map_t>::value
                && !_utils::has_to_json_in_templ_spec<map_t>::value,
            bool> = true>
    object(const map_t& m)
    {
        for (const auto& [key, val] : m) {
            _object_data.emplace(key, val);
        }
    }

    template <
        typename map_t,
        std::enable_if_t<
            _utils::is_map<map_t> && std::is_same_v<typename map_t::key_type, std::string>
                && !std::is_same_v<std::decay_t<map_t>, object> && !_utils::has_to_json_in_member<map_t>::value
                && !_utils::has_to_json_in_templ_spec<map_t>::value,
            bool> = true>
    object(map_t&& m)
    {
        for (auto& [key, val] : m) {
            _object_data.emplace(key, std::move(val));
        }
    }

    ~object();

    bool empty() const noexcept;

    size_t size() const noexcept;

    bool contains(const std::string& key) const;

    bool exists(const std::string& key) const;

    const value& at(const std::string& key) const;

    std::string dumps(std::optional<size_t> indent = std::nullopt) const;

    std::string to_string() const;

    std::string format(size_t indent = 4) const;

    template <typename value_t>
    bool all() const;

    // Unified as function for converting to maps
    template <typename T>
    T as() const&;

    template <typename T>
    T as() &&;

    // Usage: get(key_1, key_2, ..., default_value);
    template <typename... key_then_default_value_t>
    auto get(key_then_default_value_t&&... keys_then_default_value) const;

    template <typename value_t = value>
    std::optional<value_t> find(const std::string& key) const;

    template <typename... args_t>
    decltype(auto) emplace(args_t&&... args);
    template <typename... args_t>
    decltype(auto) insert(args_t&&... args);

    void clear() noexcept;
    bool erase(const std::string& key);
    bool erase(iterator iter);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    value& operator[](const std::string& key);
    value& operator[](std::string&& key);

    object operator|(const object& rhs) const&;
    object operator|(object&& rhs) const&;
    object operator|(const object& rhs) &&;
    object operator|(object&& rhs) &&;

    object& operator|=(const object& rhs);
    object& operator|=(object&& rhs);

    object& operator=(const object&);
    object& operator=(object&&) noexcept;

    template <typename value_t, std::enable_if_t<std::is_convertible_v<value_t, object>, bool> = true>
    object& operator=(value_t rhs)
    {
        return *this = object(std::move(rhs));
    }

    bool operator==(const object& rhs) const;

    bool operator!=(const object& rhs) const;

    template <typename jsonization_t, std::enable_if_t<_utils::has_from_json_in_member<jsonization_t>::value, bool> = true>
    explicit operator jsonization_t() const&
    {
        jsonization_t dst {};
        if (!dst.from_json(*this)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <
        typename jsonization_t,
        std::enable_if_t<_utils::has_from_json_in_templ_spec<std::decay_t<jsonization_t>>::value, bool> = true>
    explicit operator jsonization_t() const&
    {
        jsonization_t dst {};
        if (!ext::jsonization<std::decay_t<jsonization_t>>().from_json(*this, dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <
        typename jsonization_t,
        std::enable_if_t<_utils::has_move_from_json_in_templ_spec<std::decay_t<jsonization_t>>::value, bool> = true>
    explicit operator jsonization_t() &&
    {
        jsonization_t dst {};
        if (!ext::jsonization<std::decay_t<jsonization_t>>().move_from_json(std::move(*this), dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    // Unified native support for converting to maps
    template <
        typename T,
        std::enable_if_t<
            _utils::is_map<T> && std::is_same_v<typename T::key_type, std::string>
                && !_utils::has_from_json_in_member<T>::value && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() const&
    {
        return as<T>();
    }

    template <
        typename T,
        std::enable_if_t<
            _utils::is_map<T> && std::is_same_v<typename T::key_type, std::string>
                && !_utils::has_from_json_in_member<T>::value && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() &&
    {
        return std::move(*this).as<T>();
    }

private:
    template <typename... key_then_default_value_t, size_t... keys_indexes_t>
    auto get(std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const;
    template <typename value_t, typename... rest_keys_t>
    auto get_helper(const value_t& default_value, const std::string& key, rest_keys_t&&... rest) const;
    template <typename value_t>
    auto get_helper(const value_t& default_value, const std::string& key) const;

    std::string format(size_t indent, size_t indent_times) const;

private:
    raw_object _object_data;
};
} // namespace json
