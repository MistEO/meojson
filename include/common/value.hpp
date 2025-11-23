// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <cstddef>
#include <initializer_list>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "exception.hpp"
#include "utils.hpp"

namespace json
{
class value
{
    using array_ptr = std::unique_ptr<array>;
    using object_ptr = std::unique_ptr<object>;

public:
    enum class value_type : char
    {
        invalid,
        null,
        boolean,
        string,
        number,
        array,
        object
    };

    using var_t = std::variant<std::string, array_ptr, object_ptr>;

public:
    value();
    value(const value& rhs);
    value(value&& rhs) noexcept;

    value(bool b);

    value(int num);
    value(unsigned num);
    value(long num);
    value(unsigned long num);
    value(long long num);
    value(unsigned long long num);
    value(float num);
    value(double num);
    value(long double num);

    value(const char* str);
    value(std::string str);
    value(std::string_view str);
    value(std::nullptr_t);

    value(std::initializer_list<typename object::value_type> init_list);

    // Constructed from raw data
    template <typename... args_t>
    value(value_type type, args_t&&... args);

    template <typename enum_t, std::enable_if_t<std::is_enum_v<enum_t>, bool> = true>
    value(enum_t e)
        : value(static_cast<std::underlying_type_t<enum_t>>(e))
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<!std::is_same_v<std::decay_t<jsonization_t>, value> && _utils::has_to_json_in_member<jsonization_t>::value, bool> =
            true>
    value(const jsonization_t& val)
        : value(val.to_json())
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, value> && _utils::has_to_json_in_templ_spec<std::decay_t<jsonization_t>>::value,
            bool> = true>
    value(const jsonization_t& val)
        : value(ext::jsonization<std::decay_t<jsonization_t>>().to_json(val))
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, value> && std::is_rvalue_reference_v<jsonization_t&&>
                && _utils::has_move_to_json_in_templ_spec<std::decay_t<jsonization_t>>::value,
            bool> = true>
    value(jsonization_t&& val)
        : value(ext::jsonization<std::decay_t<jsonization_t>>().move_to_json(std::move(val)))
    {
    }

    // template <
    //     typename array_t,
    //     std::enable_if_t<
    //         std::is_constructible_v<array, array_t> && !std::is_same_v<std::decay_t<array_t>, value>
    //             && !std::is_same_v<std::decay_t<array_t>, array> && !std::is_same_v<std::decay_t<array_t>, object>,
    //         bool> = true>
    // value(array_t&& val)
    //     : _type(value_type::array)
    //     , _raw_data(std::make_unique<array>(std::forward<array_t>(val)))
    // {
    // }

    // template <
    //     typename object_t,
    //     std::enable_if_t<
    //         std::is_constructible_v<object, object_t> && !std::is_same_v<std::decay_t<object_t>, value>
    //             && !std::is_same_v<std::decay_t<object_t>, array> && !std::is_same_v<std::decay_t<object_t>, object>,
    //         bool> = true>
    // value(object_t&& val)
    //     : _type(value_type::object)
    //     , _raw_data(std::make_unique<object>(std::forward<object_t>(val)))
    // {
    // }

    // I don't know if you want to convert char to string or number, so I delete these constructors.
    value(char) = delete;
    value(wchar_t) = delete;
    value(char16_t) = delete;
    value(char32_t) = delete;

    ~value();

    bool valid() const noexcept;

    bool empty() const noexcept;

    bool is_null() const noexcept;

    bool is_number() const noexcept;

    bool is_boolean() const noexcept;

    bool is_string() const noexcept;

    bool is_array() const noexcept;

    bool is_object() const noexcept;

    template <typename value_t>
    bool is() const noexcept;

    template <typename value_t>
    bool all() const;

    bool contains(const std::string& key) const;
    bool contains(size_t pos) const;

    bool exists(const std::string& key) const;

    bool exists(size_t pos) const;

    value_type type() const noexcept;

    const value& at(size_t pos) const;
    const value& at(const std::string& key) const;

    bool erase(size_t pos);
    bool erase(const std::string& key);

    // Usage: get(key_1, key_2, ..., default_value);
    template <typename... key_then_default_value_t>
    auto get(key_then_default_value_t&&... keys_then_default_value) const;

    template <typename value_t = value>
    std::optional<value_t> find(size_t pos) const;
    template <typename value_t = value>
    std::optional<value_t> find(const std::string& key) const;

    bool as_boolean() const;
    int as_integer() const;
    unsigned as_unsigned() const;
    long as_long() const;
    unsigned long as_unsigned_long() const;
    long long as_long_long() const;
    unsigned long long as_unsigned_long_long() const;
    float as_float() const;
    double as_double() const;
    long double as_long_double() const;
    std::string as_string() const;
    const array& as_array() const;
    const object& as_object() const;

    template <typename value_t>
    value_t as() const&;

    template <typename value_t>
    value_t as() &&;

    array& as_array();
    object& as_object();

    template <typename... args_t>
    decltype(auto) emplace(args_t&&... args);

    void clear() noexcept;

    std::string dumps(std::optional<size_t> indent = std::nullopt) const;

    // return raw string
    std::string to_string() const;

    std::string format(size_t indent = 4) const;

    value& operator=(const value& rhs);
    value& operator=(value&&) noexcept;

    // template <
    //     typename value_t,
    //     std::enable_if_t<std::is_constructible_v<value_t, value>, bool> = true>
    // value& operator=(value_t rhs)
    // {
    //     return *this = value(std::move(rhs));
    // }

    bool operator==(const value& rhs) const;

    bool operator!=(const value& rhs) const;

    const value& operator[](size_t pos) const;
    value& operator[](size_t pos);
    value& operator[](const std::string& key);
    value& operator[](std::string&& key);

    value operator|(const object& rhs) const&;
    value operator|(object&& rhs) const&;
    value operator|(const object& rhs) &&;
    value operator|(object&& rhs) &&;

    value& operator|=(const object& rhs);
    value& operator|=(object&& rhs);

    value operator+(const array& rhs) const&;
    value operator+(array&& rhs) const&;
    value operator+(const array& rhs) &&;
    value operator+(array&& rhs) &&;

    value& operator+=(const array& rhs);
    value& operator+=(array&& rhs);

    explicit operator bool() const;

    explicit operator int() const;

    explicit operator unsigned() const;

    explicit operator long() const;

    explicit operator unsigned long() const;

    explicit operator long long() const;

    explicit operator unsigned long long() const;

    explicit operator float() const;

    explicit operator double() const;

    explicit operator long double() const;

    explicit operator std::string() const;

    explicit operator array() const;

    explicit operator object() const;

    template <typename enum_t, std::enable_if_t<std::is_enum_v<enum_t>, bool> = true>
    explicit operator enum_t() const
    {
        return static_cast<enum_t>(static_cast<std::underlying_type_t<enum_t>>(*this));
    }

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

private:
    friend class array;
    friend class object;

    std::string format(size_t indent, size_t indent_times) const;

    static var_t deep_copy(const var_t& src);

    template <typename... key_then_default_value_t, size_t... keys_indexes_t>
    auto get(std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const;

    template <typename value_t, typename first_key_t, typename... rest_keys_t>
    auto get_helper(const value_t& default_value, first_key_t&& first, rest_keys_t&&... rest) const;
    template <typename value_t, typename unique_key_t>
    auto get_helper(const value_t& default_value, unique_key_t&& first) const;

    const std::string& as_basic_type_str() const;
    std::string& as_basic_type_str();

    value_type _type = value_type::null;
    var_t _raw_data;
};
} // namespace json
