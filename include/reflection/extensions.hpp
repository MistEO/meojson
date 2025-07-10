// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <cstddef>
#include <optional>
#include <tuple>
#include <utility>
#include <variant>

#include "../common/types.hpp"

namespace json::ext
{

template <>
class jsonization<std::nullptr_t>
{
public:
    json::value to_json(const std::nullptr_t&) const { return json::value {}; }

    bool check_json(const json::value& j) const { return j.is_null(); }

    bool from_json(const json::value& j, std::nullptr_t&) { return check_json(j); }
};

template <typename elem1_t, typename elem2_t>
class jsonization<std::pair<elem1_t, elem2_t>>
{
public:
    json::value to_json(const std::pair<elem1_t, elem2_t>& value) const
    {
        return json::array {
            json::value { value.first },
            json::value { value.second },
        };
    }

    bool check_json(const json::value& j) const
    {
        if (!j.is_array()) {
            return false;
        }
        const auto& arr = j.as_array();
        if (arr.size() != 2) {
            return false;
        }
        return arr[0].is<elem1_t>() && arr[1].is<elem2_t>();
    }

    bool from_json(const json::value& j, std::pair<elem1_t, elem2_t>& value) const
    {
        if (!j.is_array()) {
            return false;
        }
        const auto& arr = j.as_array();
        if (arr.size() != 2) {
            return false;
        }
        if (!(arr[0].is<elem1_t>() && arr[1].is<elem2_t>())) {
            return false;
        }
        value.first = arr[0].as<elem1_t>();
        value.second = arr[1].as<elem2_t>();
        return true;
    }
};

template <typename... args_t>
class jsonization<std::tuple<args_t...>>
{
public:
    using tuple_t = std::tuple<args_t...>;
    constexpr static size_t tuple_size = std::tuple_size_v<tuple_t>;

    json::value to_json(const tuple_t& value) const
    {
        json::array result;
        to_json_impl(result, value, std::make_index_sequence<tuple_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(json::array& arr, const tuple_t& t, std::index_sequence<Is...>) const
    {
        (arr.emplace_back(std::get<Is>(t)), ...);
    }

    bool check_json(const json::value& j) const
    {
        if (!j.is_array()) {
            return false;
        }
        const auto& arr = j.as_array();
        if (arr.size() != tuple_size) {
            return false;
        }

        return check_json_impl(arr, std::make_index_sequence<tuple_size>());
    }

    template <std::size_t... Is>
    bool check_json_impl(const json::array& arr, std::index_sequence<Is...>) const
    {
        return (arr[Is].is<std::tuple_element_t<Is, tuple_t>>() && ...);
    }

    bool from_json(const json::value& j, tuple_t& value) const
    {
        if (!j.is_array()) {
            return false;
        }
        const auto& arr = j.as_array();
        if (arr.size() != tuple_size) {
            return false;
        }

        if (!check_json_impl(arr, std::make_index_sequence<tuple_size>())) {
            return false;
        }

        from_json_impl(arr, value, std::make_index_sequence<tuple_size>());
        return true;
    }

    template <std::size_t... Is>
    void from_json_impl(const json::array& arr, tuple_t& t, std::index_sequence<Is...>) const
    {
        ((std::get<Is>(t) = arr[Is].as<std::tuple_element_t<Is, tuple_t>>()), ...);
    }
};

template <typename... args_t>
class jsonization<std::variant<args_t...>>
{
public:
    using variant_t = std::variant<args_t...>;
    constexpr static size_t variant_size = std::variant_size_v<variant_t>;

    json::value to_json(const variant_t& value) const
    {
        json::value result;
        to_json_impl(result, value, std::make_index_sequence<variant_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(json::value& val, const variant_t& t, std::index_sequence<Is...>) const
    {
        ((t.index() == Is ? (val = std::get<Is>(t), true) : false) || ...);
    }

    bool check_json(const json::value& j) const
    {
        return check_json_impl(j, std::make_index_sequence<variant_size>());
    }

    template <std::size_t... Is>
    bool check_json_impl(const json::value& val, std::index_sequence<Is...>) const
    {
        return (val.is<std::variant_alternative_t<Is, variant_t>>() || ...);
    }

    bool from_json(const json::value& j, variant_t& value) const
    {
        if (!check_json_impl(j, std::make_index_sequence<variant_size>())) {
            return false;
        }

        from_json_impl(j, value, std::make_index_sequence<variant_size>());
        return true;
    }

    template <std::size_t... Is>
    void from_json_impl(const json::value& j, variant_t& t, std::index_sequence<Is...>) const
    {
        std::ignore =
            ((j.is<std::variant_alternative_t<Is, variant_t>>()
                  ? (t = j.as<std::variant_alternative_t<Is, variant_t>>(), true)
                  : false)
             || ...);
    }
};

}
