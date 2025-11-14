// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <cstddef>
#include <filesystem>
#include <queue>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "../common/types.hpp"

namespace json::ext
{
template <typename impl_t, typename var_t, size_t len> // (size_t)-1 for no
                                                       // restriction
class __jsonization_array
{
public:
    value to_json(const var_t& value) const { return static_cast<const impl_t*>(this)->to_json_array(value); }

    bool check_json(const value& json) const
    {
        if (!json.is_array()) {
            return false;
        }
        const auto& arr = json.as_array();
        if constexpr (len != static_cast<size_t>(-1)) {
            if (len != arr.size()) {
                return false;
            }
        }
        return static_cast<const impl_t*>(this)->check_json_array(arr);
    }

    bool from_json(const value& json, var_t& value) const
    {
        if (!json.is_array()) {
            return false;
        }
        const auto& arr = json.as_array();
        if constexpr (len != static_cast<size_t>(-1)) {
            if (len != arr.size()) {
                return false;
            }
        }
        return static_cast<const impl_t*>(this)->from_json_array(arr, value);
    }

    value move_to_json(var_t value) const { return static_cast<const impl_t*>(this)->move_to_json_array(std::move(value)); }

    bool move_from_json(value json, var_t& value) const
    {
        if (!json.is_array()) {
            return false;
        }
        auto& arr = json.as_array();
        if constexpr (len != static_cast<size_t>(-1)) {
            if (len != arr.size()) {
                return false;
            }
        }
        return static_cast<const impl_t*>(this)->move_from_json_array(std::move(arr), value);
    }
};

template <typename impl_t, typename var_t>
class __jsonization_object
{
public:
    value to_json(const var_t& value) const { return static_cast<const impl_t*>(this)->to_json_object(value); }

    bool check_json(const value& json) const
    {
        if (!json.is_object()) {
            return false;
        }
        const auto& obj = json.as_object();
        return static_cast<const impl_t*>(this)->check_json_object(obj);
    }

    bool from_json(const value& json, var_t& value) const
    {
        if (!json.is_object()) {
            return false;
        }
        const auto& obj = json.as_object();
        return static_cast<const impl_t*>(this)->from_json_object(obj, value);
    }

    value move_to_json(var_t value) const { return static_cast<const impl_t*>(this)->move_to_json_object(std::move(value)); }

    bool move_from_json(value json, var_t& value) const
    {
        if (!json.is_object()) {
            return false;
        }
        auto& obj = json.as_object();
        return static_cast<const impl_t*>(this)->move_from_json_object(std::move(obj), value);
    }
};

template <>
class jsonization<std::nullptr_t>
{
public:
    value to_json(const std::nullptr_t&) const { return value {}; }

    bool check_json(const value& json) const { return json.is_null(); }

    bool from_json(const value& json, std::nullptr_t&) { return check_json(json); }
};

template <template <typename, size_t> typename arr_t, typename value_t, size_t size>
class jsonization<arr_t<value_t, size>> : public __jsonization_array<jsonization<arr_t<value_t, size>>, arr_t<value_t, size>, size>
{
public:
    array to_json_array(const arr_t<value_t, size>& value) const
    {
        json::array result;
        for (size_t i = 0; i < size; i++) {
            result.emplace_back(value.at(i));
        }
        return result;
    }

    bool check_json_array(const array& arr) const { return arr.template all<value_t>(); }

    bool from_json_array(const array& arr, arr_t<value_t, size>& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            value.at(i) = arr[i].template as<value_t>();
        }
        return true;
    }

    array move_to_json_array(arr_t<value_t, size> value) const
    {
        json::array result;
        for (size_t i = 0; i < size; i++) {
            result.emplace_back(std::move(value.at(i)));
        }
        return result;
    }

    bool move_from_json_array(array arr, arr_t<value_t, size>& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            value.at(i) = std::move(arr[i]).template as<value_t>();
        }
        return true;
    }
};

template <typename collection_t>
class jsonization<collection_t, std::enable_if_t<_utils::is_collection<collection_t>>>
    : public __jsonization_array<jsonization<collection_t>, collection_t, (size_t)-1>
{
public:
    array to_json_array(const collection_t& value) const
    {
        json::array result;
        for (const auto& val : value) {
            result.emplace_back(val);
        }
        return result;
    }

    bool check_json_array(const array& arr) const { return arr.template all<typename collection_t::value_type>(); }

    bool from_json_array(const array& arr, collection_t& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        value = {};
        for (const auto& val : arr) {
            if constexpr (_utils::has_emplace_back<collection_t>::value) {
                value.emplace_back(val.template as<typename collection_t::value_type>());
            }
            else {
                value.emplace(val.template as<typename collection_t::value_type>());
            }
        }
        return true;
    }

    array move_to_json_array(collection_t value) const
    {
        json::array result;
        for (auto& val : value) {
            result.emplace_back(std::move(val));
        }
        return result;
    }

    bool move_from_json_array(array arr, collection_t& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        for (auto& val : arr) {
            if constexpr (_utils::has_emplace_back<collection_t>::value) {
                value.emplace_back(std::move(val).template as<typename collection_t::value_type>());
            }
            else {
                value.emplace(std::move(val).template as<typename collection_t::value_type>());
            }
        }
        return true;
    }
};

template <template <typename...> typename tuple_t, typename... args_t>
class jsonization<tuple_t<args_t...>, std::enable_if_t<_utils::is_tuple_like<tuple_t<args_t...>>>>
    : public __jsonization_array<jsonization<tuple_t<args_t...>>, tuple_t<args_t...>, std::tuple_size_v<tuple_t<args_t...>>>
{
public:
    constexpr static size_t tuple_size = std::tuple_size_v<tuple_t<args_t...>>;

    array to_json_array(const tuple_t<args_t...>& value) const
    {
        json::array result;
        to_json_impl(result, value, std::make_index_sequence<tuple_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(array& arr, const tuple_t<args_t...>& t, std::index_sequence<Is...>) const
    {
        using std::get;
        (arr.emplace_back(get<Is>(t)), ...);
    }

    bool check_json_array(const array& arr) const { return check_json_impl(arr, std::make_index_sequence<tuple_size>()); }

    template <std::size_t... Is>
    bool check_json_impl(const array& arr, std::index_sequence<Is...>) const
    {
        return (arr[Is].template is<std::tuple_element_t<Is, tuple_t<args_t...>>>() && ...);
    }

    bool from_json_array(const array& arr, tuple_t<args_t...>& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        from_json_impl(arr, value, std::make_index_sequence<tuple_size>());
        return true;
    }

    template <std::size_t... Is>
    void from_json_impl(const array& arr, tuple_t<args_t...>& t, std::index_sequence<Is...>) const
    {
        using std::get;
        ((get<Is>(t) = arr[Is].template as<std::tuple_element_t<Is, tuple_t<args_t...>>>()), ...);
    }

    array move_to_json_array(tuple_t<args_t...> value) const
    {
        json::array result;
        move_to_json_impl(result, std::move(value), std::make_index_sequence<tuple_size>());
        return result;
    }

    template <std::size_t... Is>
    void move_to_json_impl(array& arr, tuple_t<args_t...> t, std::index_sequence<Is...>) const
    {
        using std::get;
        (arr.emplace_back(std::move(get<Is>(t))), ...);
    }

    bool move_from_json_array(array arr, tuple_t<args_t...>& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        move_from_json_impl(arr, value, std::make_index_sequence<tuple_size>());
        return true;
    }

    template <std::size_t... Is>
    void move_from_json_impl(array arr, tuple_t<args_t...>& t, std::index_sequence<Is...>) const
    {
        using std::get;
        ((get<Is>(t) = std::move(arr[Is]).template as<std::tuple_element_t<Is, tuple_t<args_t...>>>()), ...);
    }
};

template <typename map_t>
class jsonization<map_t, std::enable_if_t<_utils::is_map<map_t> && std::is_same_v<typename map_t::key_type, std::string>>>
    : public __jsonization_object<jsonization<map_t>, map_t>
{
public:
    object to_json_object(const map_t& value) const
    {
        json::object result;
        for (const auto& [key, val] : value) {
            result.emplace(key, val);
        }
        return result;
    }

    bool check_json_object(const object& arr) const
    {
        for (const auto& [key, val] : arr) {
            if (!val.template is<typename map_t::mapped_type>()) {
                return false;
            }
        }
        return true;
    }

    bool from_json_object(const object& arr, map_t& value) const
    {
        // TODO: 是不是直接from不check了算了
        if (!check_json_object(arr)) {
            return false;
        }

        value = {};
        for (const auto& [key, val] : arr) {
            value.emplace(key, val.template as<typename map_t::mapped_type>());
        }
        return true;
    }

    object move_to_json_object(map_t value) const
    {
        json::object result;
        for (auto& [key, val] : value) {
            result.emplace(key, std::move(val));
        }
        return result;
    }

    bool move_from_json_object(object arr, map_t& value) const
    {
        // TODO: 是不是直接from不check了算了
        if (!check_json_object(arr)) {
            return false;
        }

        value = {};
        for (auto& [key, val] : arr) {
            value.emplace(key, std::move(val).template as<typename map_t::mapped_type>());
        }
        return true;
    }
};

template <typename... args_t>
class jsonization<std::variant<args_t...>>
{
public:
    using variant_t = std::variant<args_t...>;
    constexpr static size_t variant_size = std::variant_size_v<variant_t>;

    value to_json(const variant_t& value) const
    {
        json::value result;
        to_json_impl(result, value, std::make_index_sequence<variant_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(value& val, const variant_t& t, std::index_sequence<Is...>) const
    {
        using std::get;
        std::ignore = ((t.index() == Is ? (val = get<Is>(t), true) : false) || ...);
    }

    bool check_json(const value& json) const { return check_json_impl(json, std::make_index_sequence<variant_size>()); }

    template <std::size_t... Is>
    bool check_json_impl(const value& val, std::index_sequence<Is...>) const
    {
        return (val.template is<std::variant_alternative_t<Is, variant_t>>() || ...);
    }

    bool from_json(const value& json, variant_t& value) const
    {
        if (!check_json_impl(json, std::make_index_sequence<variant_size>())) {
            return false;
        }

        from_json_impl(json, value, std::make_index_sequence<variant_size>());
        return true;
    }

    template <std::size_t... Is>
    void from_json_impl(const value& json, variant_t& t, std::index_sequence<Is...>) const
    {
        std::ignore =
            ((json.template is<std::variant_alternative_t<Is, variant_t>>()
                  ? (t = json.template as<std::variant_alternative_t<Is, variant_t>>(), true)
                  : false)
             || ...);
    }

    value move_to_json(variant_t value) const
    {
        json::value result;
        move_to_json_impl(result, std::move(value), std::make_index_sequence<variant_size>());
        return result;
    }

    template <std::size_t... Is>
    void move_to_json_impl(value& val, variant_t t, std::index_sequence<Is...>) const
    {
        using std::get;
        std::ignore = ((t.index() == Is ? (val = std::move(get<Is>(t)), true) : false) || ...);
    }

    bool move_from_json(value json, variant_t& value) const
    {
        if (!check_json_impl(json, std::make_index_sequence<variant_size>())) {
            return false;
        }

        move_from_json_impl(std::move(json), value, std::make_index_sequence<variant_size>());
        return true;
    }

    template <std::size_t... Is>
    void move_from_json_impl(value json, variant_t& t, std::index_sequence<Is...>) const
    {
        std::ignore =
            ((json.template is<std::variant_alternative_t<Is, variant_t>>()
                  ? (t = std::move(json).template as<std::variant_alternative_t<Is, variant_t>>(), true)
                  : false)
             || ...);
    }
};

// TODO: check if has move_xxx in member
template <typename var_t>
class jsonization<
    var_t,
    std::enable_if_t<
        _utils::has_to_json_in_member<var_t>::value && _utils::has_check_json_in_member<var_t>::value
        && _utils::has_from_json_in_member<var_t>::value>>
{
public:
    value to_json(const var_t& value) const { return value.to_json(); }

    bool check_json(const value& json) const
    {
        var_t value;
        return value.check_json(json);
    }

    bool from_json(const value& json, var_t& value) const { return value.from_json(json); }

    value move_to_json(var_t value) const { return to_json(value); }

    bool move_from_json(value json, var_t& value) const { return from_json(json, value); }
};

#ifdef MEOJSON_FS_PATH_EXTENSION
template <>
class jsonization<std::filesystem::path>
{
public:
    value to_json(const std::filesystem::path& path) const
    {
        if constexpr (std::is_same_v<std::string, std::filesystem::path::string_type>) {
            return path.native();
        }
        else {
#if __cplusplus >= 202002L
            std::u8string u8str = path.u8string();
            return std::string { u8str.begin(), u8str.end() };
#else
            return path.u8string();
#endif
        }
    }

    bool check_json(const value& json) const { return json.is_string(); }

    bool from_json(const value& json, std::filesystem::path& path) const
    {
        path = json.as_string();
        return true;
    }
};
#endif
}
