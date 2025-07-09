// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <optional>
#include <tuple>
#include <utility>

#include "../common/types.hpp"

namespace json
{

template <typename value_t>
struct meo_opt : public std::optional<value_t>
{
    using std::optional<value_t>::optional;
};

}

namespace json::ext
{

template <typename value_t>
class jsonization<std::optional<value_t>>
{
public:
    json::value to_json(const std::optional<value_t>& value) const
    {
        return value.has_value() ? json::value {} : json::value { value.value() };
    }

    bool check_json(const json::value& j) const
    {
        if (j.is_null()) {
            return true;
        }
        return j.is<value_t>();
    }

    bool from_json(const json::value& j, std::optional<value_t>& value) const
    {
        if (j.is_null()) {
            value = std::nullopt;
            return true;
        }
        if (!j.is<value_t>()) {
            return false;
        }
        value.value() = value_t { j };
        return true;
    }
};

template <typename value_t>
class jsonization<meo_opt<value_t>>
{
public:
    json::value to_json(const meo_opt<value_t>& value) const
    {
        return value.has_value() ? json::value {} : json::value { value.value() };
    }

    bool check_json(const json::value& j) const
    {
        if (j.is_null()) {
            return true;
        }
        return j.is<value_t>();
    }

    bool from_json(const json::value& j, meo_opt<value_t>& value) const
    {
        if (j.is_null()) {
            value = std::nullopt;
            return true;
        }
        if (!j.is<value_t>()) {
            return false;
        }
        value.value() = value_t { j };
        return true;
    }
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
    using tuple_t = std::tuple<args_t...>;
    constexpr static size_t tuple_size = std::tuple_size_v<tuple_t>;

    json::value to_json(const tuple_t& value) const
    {
        json::array result;
        to_json_impl(result, value, std::make_index_sequence<tuple_size>());
        return result;
    }

    template <std::size_t... Is>
    void to_json_impl(json::array& arr, const tuple_t& t, std::index_sequence<Is...>)
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
    bool check_json_impl(const json::array& arr, std::index_sequence<Is...>)
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
    void from_json_impl(const json::array& arr, tuple_t& t, std::index_sequence<Is...>)
    {
        return ((std::get<Is>(t) = arr[Is]), ...);
    }
};

}
