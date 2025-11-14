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
    object() = default;
    object(const object& rhs) = default;
    object(object&& rhs) noexcept = default;
    object(std::initializer_list<value_type> init_list);

    // explicit object(const value& val);
    // explicit object(value&& val);

    template <
        typename jsonization_t,
        std::enable_if_t<
            _utils::has_to_json_in_templ_spec<jsonization_t>::value && !_utils::has_to_json_object_in_templ_spec<jsonization_t>::value,
            bool> = true>
    object(const jsonization_t& value)
        : object(ext::jsonization<jsonization_t>().to_json(value))
    {
    }

    template <typename jsonization_t, std::enable_if_t<_utils::has_to_json_object_in_templ_spec<jsonization_t>::value, bool> = true>
    object(const jsonization_t& value)
        : object(ext::jsonization<jsonization_t>().to_json_object(value))
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            std::is_rvalue_reference_v<jsonization_t&&> && _utils::has_move_to_json_in_templ_spec<jsonization_t>::value
                && !_utils::has_move_to_json_object_in_templ_spec<jsonization_t>::value,
            bool> = true>
    object(jsonization_t&& value)
        : object(ext::jsonization<jsonization_t>().move_to_json(std::move(value)))
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            std::is_rvalue_reference_v<jsonization_t&&> && _utils::has_move_to_json_object_in_templ_spec<jsonization_t>::value,
            bool> = true>
    object(jsonization_t&& value)
        : object(ext::jsonization<jsonization_t>().move_to_json_object(std::move(value)))
    {
    }

    ~object() = default;

    bool empty() const noexcept { return _object_data.empty(); }

    size_t size() const noexcept { return _object_data.size(); }

    bool contains(const std::string& key) const;

    bool exists(const std::string& key) const { return contains(key); }

    const value& at(const std::string& key) const;

    std::string dumps(std::optional<size_t> indent = std::nullopt) const { return indent ? format(*indent) : to_string(); }

    std::string to_string() const;

    std::string format(size_t indent = 4) const { return format(indent, 0); }

    template <typename value_t>
    bool all() const;

    template <typename value_t, std::enable_if_t<_utils::has_from_json_object_in_templ_spec<value_t>::value, bool> = true>
    value_t as() const&
    {
        value_t res;
        ext::jsonization<value_t>().from_json_object(*this, res);
        return res;
    }

    template <typename value_t, std::enable_if_t<_utils::has_move_from_json_object_in_templ_spec<value_t>::value, bool> = true>
    value_t as() &&
    {
        value_t res;
        ext::jsonization<value_t>().move_from_json_object(std::move(*this), res);
        return res;
    }

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

    object& operator=(const object&) = default;
    object& operator=(object&&) = default;

    template <typename value_t, std::enable_if_t<std::is_convertible_v<value_t, object>, bool> = true>
    object& operator=(value_t rhs)
    {
        return *this = object(std::move(rhs));
    }

    bool operator==(const object& rhs) const;

    bool operator!=(const object& rhs) const { return !(*this == rhs); }

    template <
        typename jsonization_t,
        std::enable_if_t<
            _utils::has_from_json_in_templ_spec<jsonization_t>::value && !_utils::has_from_json_object_in_templ_spec<jsonization_t>::value,
            bool> = true>
    explicit operator jsonization_t() const&
    {
        jsonization_t dst {};
        if (!ext::jsonization<jsonization_t>().from_json(*this, dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <typename jsonization_t, std::enable_if_t<_utils::has_from_json_object_in_templ_spec<jsonization_t>::value, bool> = true>
    explicit operator jsonization_t() const&
    {
        jsonization_t dst {};
        if (!ext::jsonization<jsonization_t>().from_json_object(*this, dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            _utils::has_move_from_json_in_templ_spec<jsonization_t>::value
                && !_utils::has_move_from_json_object_in_templ_spec<jsonization_t>::value,
            bool> = true>
    explicit operator jsonization_t() &&
    {
        jsonization_t dst {};
        if (!ext::jsonization<jsonization_t>().from_json(std::move(*this), dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <typename jsonization_t, std::enable_if_t<_utils::has_move_from_json_object_in_templ_spec<jsonization_t>::value, bool> = true>
    explicit operator jsonization_t() &&
    {
        jsonization_t dst {};
        if (!ext::jsonization<jsonization_t>().move_from_json_object(std::move(*this), dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
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