// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <cstddef>
#include <filesystem>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include "../common/types.hpp"

namespace json::ext
{

template <typename string_t, typename impl_t, typename var_t, size_t len> // (size_t)-1 for no
                                                                          // restriction
class __jsonization_array
{
public:
    json::basic_value<string_t> to_json(const var_t& value) const
    {
        return static_cast<const impl_t*>(this)->to_json_array(value);
    }

    bool check_json(const json::basic_value<string_t>& json) const
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

    bool from_json(const json::basic_value<string_t>& json, var_t& value) const
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
};

// template <typename impl_t, typename var_t>
// class __jsonization_object
// {
// public:
//
//     json::basic_value<string_t> to_json(const var_t& value) const
//     {
//         return static_cast<const impl_t*>(this)->template to_json_object<string_t>(value);
//     }

//
//     bool check_json(const json::basic_value<string_t>& json) const
//     {
//         if (!json.is_object()) {
//             return false;
//         }
//         const auto& obj = json.as_object();
//         return static_cast<const impl_t*>(this)->check_json_object(obj);
//     }

//
//     bool from_json(const json::basic_value<string_t>& json, var_t& value) const
//     {
//         if (!json.is_object()) {
//             return false;
//         }
//         const auto& obj = json.as_object();
//         return static_cast<const impl_t*>(this)->from_json_object(obj, value);
//     }
// };

template <typename string_t>
class jsonization<string_t, std::nullptr_t>
{
public:
    json::basic_value<string_t> to_json(const std::nullptr_t&) const
    {
        return json::basic_value<string_t> {};
    }

    bool check_json(const json::basic_value<string_t>& json) const { return json.is_null(); }

    bool from_json(const json::basic_value<string_t>& json, std::nullptr_t&)
    {
        return check_json(json);
    }
};

template <typename string_t>
class jsonization<
    string_t,
    std::filesystem::path,
    std::enable_if_t<
        std::is_same_v<string_t, std::filesystem::path::string_type>
        || std::is_same_v<string_t, std::string>>>
{
public:
    json::basic_value<string_t> to_json(const std::filesystem::path& path) const
    {
        if constexpr (std::is_same_v<string_t, std::filesystem::path::string_type>) {
            return path.native();
        }
        else if constexpr (std::is_same_v<string_t, std::string>) {
            return path.string();
        }
    }

    bool check_json(const json::basic_value<string_t>& json) const { return json.is_string(); }

    bool from_json(const json::basic_value<string_t>& json, std::filesystem::path& path) const
    {
        path = json.as_string();
        return true;
    }
};

template <
    typename string_t,
    template <typename, size_t> typename arr_t,
    typename value_t,
    size_t size>
class jsonization<string_t, arr_t<value_t, size>>
    : public __jsonization_array<
          string_t,
          jsonization<string_t, arr_t<value_t, size>>,
          arr_t<value_t, size>,
          size>
{
public:
    json::basic_array<string_t> to_json_array(const arr_t<value_t, size>& value) const
    {
        json::basic_array<string_t> result;
        for (size_t i = 0; i < size; i++) {
            result.emplace_back(value.at(i));
        }
        return result;
    }

    bool check_json_array(const json::basic_array<string_t>& arr) const
    {
        return arr.template all<value_t>();
    }

    bool from_json_array(const json::basic_array<string_t>& arr, arr_t<value_t, size>& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            value.at(i) = arr[i].template as<value_t>();
        }
        return true;
    }
};

template <typename string_t, template <typename...> typename tuple_t, typename... args_t>
class jsonization<
    string_t,
    tuple_t<args_t...>,
    std::enable_if_t<_utils::is_tuple_like<tuple_t<args_t...>>>>
    : public __jsonization_array<
          string_t,
          jsonization<string_t, tuple_t<args_t...>>,
          tuple_t<args_t...>,
          std::tuple_size_v<tuple_t<args_t...>>>
{
public:
    constexpr static size_t tuple_size = std::tuple_size_v<tuple_t<args_t...>>;

    json::basic_array<string_t> to_json_array(const tuple_t<args_t...>& value) const
    {
        json::basic_array<string_t> result;
        to_json_impl(result, value, std::make_index_sequence<tuple_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(
        json::basic_array<string_t>& arr,
        const tuple_t<args_t...>& t,
        std::index_sequence<Is...>) const
    {
        using std::get;
        (arr.emplace_back(get<Is>(t)), ...);
    }

    bool check_json_array(const json::basic_array<string_t>& arr) const
    {
        return check_json_impl(arr, std::make_index_sequence<tuple_size>());
    }

    template <std::size_t... Is>
    bool check_json_impl(const json::basic_array<string_t>& arr, std::index_sequence<Is...>) const
    {
        return (arr[Is].template is<std::tuple_element_t<Is, tuple_t<args_t...>>>() && ...);
    }

    bool from_json_array(const json::basic_array<string_t>& arr, tuple_t<args_t...>& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        from_json_impl(arr, value, std::make_index_sequence<tuple_size>());
        return true;
    }

    template <std::size_t... Is>
    void from_json_impl(
        const json::basic_array<string_t>& arr,
        tuple_t<args_t...>& t,
        std::index_sequence<Is...>) const
    {
        using std::get;
        ((get<Is>(t) = arr[Is].template as<std::tuple_element_t<Is, tuple_t<args_t...>>>()), ...);
    }
};

template <typename string_t, typename... args_t>
class jsonization<string_t, std::variant<args_t...>>
{
public:
    using variant_t = std::variant<args_t...>;
    constexpr static size_t variant_size = std::variant_size_v<variant_t>;

    json::basic_value<string_t> to_json(const variant_t& value) const
    {
        json::basic_value<string_t> result;
        to_json_impl(result, value, std::make_index_sequence<variant_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(
        json::basic_value<string_t>& val,
        const variant_t& t,
        std::index_sequence<Is...>) const
    {
        using std::get;
        std::ignore = ((t.index() == Is ? (val = get<Is>(t), true) : false) || ...);
    }

    bool check_json(const json::basic_value<string_t>& json) const
    {
        return check_json_impl(json, std::make_index_sequence<variant_size>());
    }

    template <std::size_t... Is>
    bool check_json_impl(const json::basic_value<string_t>& val, std::index_sequence<Is...>) const
    {
        return (val.template is<std::variant_alternative_t<Is, variant_t>>() || ...);
    }

    bool from_json(const json::basic_value<string_t>& json, variant_t& value) const
    {
        if (!check_json_impl(json, std::make_index_sequence<variant_size>())) {
            return false;
        }

        from_json_impl(json, value, std::make_index_sequence<variant_size>());
        return true;
    }

    template <std::size_t... Is>
    void from_json_impl(
        const json::basic_value<string_t>& json,
        variant_t& t,
        std::index_sequence<Is...>) const
    {
        std::ignore =
            ((json.template is<std::variant_alternative_t<Is, variant_t>>()
                  ? (t = json.template as<std::variant_alternative_t<Is, variant_t>>(), true)
                  : false)
             || ...);
    }
};

template <typename string_t, typename var_t>
class jsonization<
    string_t,
    var_t,
    std::enable_if_t<
        _utils::has_to_json_in_member<var_t>::value
        && _utils::has_check_json_in_member<var_t, string_t>::value
        && _utils::has_from_json_in_member<var_t, string_t>::value>>
{
public:
    json::basic_value<string_t> to_json(const var_t& value) const { return value.to_json(); }

    bool check_json(const json::basic_value<string_t>& json) const
    {
        var_t value;
        return value.check_json(json);
    }

    bool from_json(const json::basic_value<string_t>& json, var_t& value) const
    {
        return value.from_json(json);
    }
};

}
