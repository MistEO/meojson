// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#ifdef MEOJSON_FS_PATH_EXTENSION
#include <filesystem>
#endif

#include "enum_reflection.hpp"
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
    value(std::monostate);

    value(const array& arr);
    value(array&& arr);
    value(const object& obj);
    value(object&& obj);

    value(std::initializer_list<typename object::value_type> init_list);

    // Constructed from raw data
    template <typename... args_t>
    value(value_type type, args_t&&... args);

    template <typename enum_t, std::enable_if_t<std::is_enum_v<enum_t>, bool> = true>
    value(enum_t e)
#ifdef MEOJSON_ENUM_AS_NUMBER
        : value(static_cast<std::underlying_type_t<enum_t>>(e))
#else
        : value(_reflection::enum_to_string(e))
#endif
    {
#ifndef MEOJSON_ENUM_AS_NUMBER
        if (as_string().empty()) {
            throw exception("Unknown Enum Value");
        }
#endif
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

    // Native support for nullable wrappers (std::optional, std::shared_ptr, std::unique_ptr)
    template <
        typename wrapper_t,
        std::enable_if_t<
            _utils::is_nullable<std::decay_t<wrapper_t>> && std::is_constructible_v<value, typename _utils::nullable_value_t<wrapper_t>>
                && !_utils::has_to_json_in_member<wrapper_t>::value && !_utils::has_to_json_in_templ_spec<wrapper_t>::value,
            bool> = true>
    value(const wrapper_t& wrapper)
        : value(wrapper ? value(*wrapper) : value())
    {
    }

    template <
        typename wrapper_t,
        std::enable_if_t<
            _utils::is_nullable<std::decay_t<wrapper_t>> && std::is_constructible_v<value, typename _utils::nullable_value_t<wrapper_t>>
                && !_utils::has_to_json_in_member<wrapper_t>::value && !_utils::has_to_json_in_templ_spec<wrapper_t>::value,
            bool> = true>
    value(wrapper_t&& wrapper)
        : value(wrapper ? value(std::move(*wrapper)) : value())
    {
    }

    // Native support for std::variant<Ts...>
    template <
        typename... Ts,
        std::enable_if_t<
            (sizeof...(Ts) > 0) && !_utils::has_to_json_in_member<std::variant<Ts...>>::value
                && !_utils::has_to_json_in_templ_spec<std::variant<Ts...>>::value,
            bool> = true>
    value(const std::variant<Ts...>& var);

    template <
        typename... Ts,
        std::enable_if_t<
            (sizeof...(Ts) > 0) && !_utils::has_to_json_in_member<std::variant<Ts...>>::value
                && !_utils::has_to_json_in_templ_spec<std::variant<Ts...>>::value,
            bool> = true>
    value(std::variant<Ts...>&& var);

    // Native support for fixed-size arrays (std::array<T, N>)
    template <
        template <typename, size_t> typename arr_t,
        typename value_t,
        size_t size,
        std::enable_if_t<
            _utils::is_fixed_array<arr_t<value_t, size>> && !std::is_same_v<std::decay_t<arr_t<value_t, size>>, value>
                && !_utils::has_to_json_in_member<arr_t<value_t, size>>::value
                && !_utils::has_to_json_in_templ_spec<arr_t<value_t, size>>::value,
            bool> = true>
    value(const arr_t<value_t, size>& arr)
        : _type(value_type::array)
        , _raw_data(std::make_unique<array>(arr))
    {
    }

    template <
        template <typename, size_t> typename arr_t,
        typename value_t,
        size_t size,
        std::enable_if_t<
            _utils::is_fixed_array<arr_t<value_t, size>> && !std::is_same_v<std::decay_t<arr_t<value_t, size>>, value>
                && !_utils::has_to_json_in_member<arr_t<value_t, size>>::value
                && !_utils::has_to_json_in_templ_spec<arr_t<value_t, size>>::value,
            bool> = true>
    value(arr_t<value_t, size>&& arr)
        : _type(value_type::array)
        , _raw_data(std::make_unique<array>(std::move(arr)))
    {
    }

    // Native support for collections (std::vector, std::list, std::set, etc.)
    template <
        typename collection_t,
        std::enable_if_t<
            _utils::is_collection<collection_t> && !std::is_same_v<std::decay_t<collection_t>, value>
                && !std::is_same_v<std::decay_t<collection_t>, array> && !_utils::has_to_json_in_member<collection_t>::value
                && !_utils::has_to_json_in_templ_spec<collection_t>::value,
            bool> = true>
    value(const collection_t& coll)
        : _type(value_type::array)
        , _raw_data(std::make_unique<array>(coll))
    {
    }

    template <
        typename collection_t,
        std::enable_if_t<
            _utils::is_collection<collection_t> && !std::is_same_v<std::decay_t<collection_t>, value>
                && !std::is_same_v<std::decay_t<collection_t>, array> && !_utils::has_to_json_in_member<collection_t>::value
                && !_utils::has_to_json_in_templ_spec<collection_t>::value,
            bool> = true>
    value(collection_t&& coll)
        : _type(value_type::array)
        , _raw_data(std::make_unique<array>(std::move(coll)))
    {
    }

    // Native support for tuple-like types (std::tuple, std::pair)
    template <
        template <typename...> typename tuple_t,
        typename... args_t,
        std::enable_if_t<
            _utils::is_tuple_like<tuple_t<args_t...>> && !std::is_same_v<std::decay_t<tuple_t<args_t...>>, value>
                && !_utils::has_to_json_in_member<tuple_t<args_t...>>::value
                && !_utils::has_to_json_in_templ_spec<tuple_t<args_t...>>::value,
            bool> = true>
    value(const tuple_t<args_t...>& tpl)
        : _type(value_type::array)
        , _raw_data(std::make_unique<array>(tpl))
    {
    }

    template <
        template <typename...> typename tuple_t,
        typename... args_t,
        std::enable_if_t<
            _utils::is_tuple_like<tuple_t<args_t...>> && !std::is_same_v<std::decay_t<tuple_t<args_t...>>, value>
                && !_utils::has_to_json_in_member<tuple_t<args_t...>>::value
                && !_utils::has_to_json_in_templ_spec<tuple_t<args_t...>>::value,
            bool> = true>
    value(tuple_t<args_t...>&& tpl)
        : _type(value_type::array)
        , _raw_data(std::make_unique<array>(std::move(tpl)))
    {
    }

    // Native support for maps (std::map<std::string, T>, std::unordered_map<std::string, T>)
    template <
        typename map_t,
        std::enable_if_t<
            _utils::is_map<map_t> && std::is_same_v<typename map_t::key_type, std::string> && !std::is_same_v<std::decay_t<map_t>, value>
                && !std::is_same_v<std::decay_t<map_t>, object> && !_utils::has_to_json_in_member<map_t>::value
                && !_utils::has_to_json_in_templ_spec<map_t>::value,
            bool> = true>
    value(const map_t& m)
        : _type(value_type::object)
        , _raw_data(std::make_unique<object>(m))
    {
    }

    template <
        typename map_t,
        std::enable_if_t<
            _utils::is_map<map_t> && std::is_same_v<typename map_t::key_type, std::string> && !std::is_same_v<std::decay_t<map_t>, value>
                && !std::is_same_v<std::decay_t<map_t>, object> && !_utils::has_to_json_in_member<map_t>::value
                && !_utils::has_to_json_in_templ_spec<map_t>::value,
            bool> = true>
    value(map_t&& m)
        : _type(value_type::object)
        , _raw_data(std::make_unique<object>(std::move(m)))
    {
    }

#ifdef MEOJSON_FS_PATH_EXTENSION
    // Native support for std::filesystem::path
    template <
        typename path_t,
        std::enable_if_t<
            std::is_same_v<std::decay_t<path_t>, std::filesystem::path> && !_utils::has_to_json_in_member<path_t>::value
                && !_utils::has_to_json_in_templ_spec<path_t>::value,
            bool> = true>
    value(const path_t& path)
    {
        if constexpr (std::is_same_v<std::string, std::filesystem::path::string_type>) {
            *this = value(path.native());
        }
        else {
#if __cplusplus >= 202002L
            std::u8string u8str = path.u8string();
            *this = value(std::string { u8str.begin(), u8str.end() });
#else
            *this = value(path.u8string());
#endif
        }
    }
#endif

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
    std::string_view as_string_view() const;
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

private:
    template <typename tuple_t, size_t... Is>
    bool is_tuple_helper(std::index_sequence<Is...>) const noexcept;

    template <typename... Ts>
    bool is_variant_helper(std::variant<Ts...>*) const noexcept;

    template <typename... Ts>
    std::variant<Ts...> to_variant_helper() const&;

    template <typename... Ts>
    std::variant<Ts...> move_to_variant_helper() &&;

public:
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

    explicit operator std::string_view() const&;
    explicit operator std::string_view() && = delete;

    explicit operator array() const;

    explicit operator object() const;

    explicit operator std::nullptr_t() const;
    explicit operator std::monostate() const;

    template <typename enum_t, std::enable_if_t<std::is_enum_v<enum_t>, bool> = true>
    explicit operator enum_t() const
    {
        if (is_string()) {
            if (auto enum_opt = _reflection::string_to_enum<enum_t>(as_string_view()); enum_opt) {
                return *enum_opt;
            }
            else {
                throw exception("Wrong Enum String Value:" + as_string());
            }
        }
        else if (is_number()) {
            return static_cast<enum_t>(static_cast<std::underlying_type_t<enum_t>>(*this));
        }
        else {
            throw exception("Wrong Type");
        }
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

    // Native support for converting to nullable wrappers (std::optional, std::shared_ptr, std::unique_ptr)
    template <typename T>
    explicit operator std::optional<T>() const&
    {
        if (is_null()) {
            return std::nullopt;
        }
        return std::optional<T>(static_cast<T>(*this));
    }

    template <typename T>
    explicit operator std::optional<T>() &&
    {
        if (is_null()) {
            return std::nullopt;
        }
        return std::optional<T>(static_cast<T>(std::move(*this)));
    }

    template <typename T>
    explicit operator std::shared_ptr<T>() const&
    {
        if (is_null()) {
            return nullptr;
        }
        return std::make_shared<T>(static_cast<T>(*this));
    }

    template <typename T>
    explicit operator std::shared_ptr<T>() &&
    {
        if (is_null()) {
            return nullptr;
        }
        return std::make_shared<T>(static_cast<T>(std::move(*this)));
    }

    template <typename T>
    explicit operator std::unique_ptr<T>() const&
    {
        if (is_null()) {
            return nullptr;
        }
        return std::make_unique<T>(static_cast<T>(*this));
    }

    template <typename T>
    explicit operator std::unique_ptr<T>() &&
    {
        if (is_null()) {
            return nullptr;
        }
        return std::make_unique<T>(static_cast<T>(std::move(*this)));
    }

    // Native support for converting to std::variant<Ts...>
    template <typename... Ts>
    explicit operator std::variant<Ts...>() const&;

    template <typename... Ts>
    explicit operator std::variant<Ts...>() &&;

#ifdef MEOJSON_FS_PATH_EXTENSION
    // Native support for converting to std::filesystem::path
    explicit operator std::filesystem::path() const& { return std::filesystem::path(as_string()); }

    explicit operator std::filesystem::path() && { return std::filesystem::path(as_string()); }
#endif

    // Unified native support for converting value to collections, maps, fixed-size arrays, and tuple-like types
    template <
        typename T,
        std::enable_if_t<
            (_utils::is_collection<T> || _utils::is_fixed_array<T> || _utils::is_tuple_like<T>)
                && !_utils::has_from_json_in_member<T>::value && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() const&
    {
        return as_array().template as<T>();
    }

    template <
        typename T,
        std::enable_if_t<
            (_utils::is_collection<T> || _utils::is_fixed_array<T> || _utils::is_tuple_like<T>)
                && !_utils::has_from_json_in_member<T>::value && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() &&
    {
        return std::move(as_array()).template as<T>();
    }

    template <
        typename T,
        std::enable_if_t<
            _utils::is_map<T> && std::is_same_v<typename T::key_type, std::string> && !_utils::has_from_json_in_member<T>::value
                && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() const&
    {
        return as_object().template as<T>();
    }

    template <
        typename T,
        std::enable_if_t<
            _utils::is_map<T> && std::is_same_v<typename T::key_type, std::string> && !_utils::has_from_json_in_member<T>::value
                && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() &&
    {
        return std::move(as_object()).template as<T>();
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
