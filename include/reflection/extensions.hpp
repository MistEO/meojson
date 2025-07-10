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

template <typename impl_t, typename var_t, size_t len> // (size_t)-1 for no restriction
class __jsonization_array
{
public:
    template <typename string_t>
    json::basic_value<string_t> to_json(const var_t& value) const
    {
        return static_cast<const impl_t*>(this)->template to_json_array<string_t>(value);
    }

    template <typename string_t>
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

    template <typename string_t>
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

template <>
class jsonization<std::nullptr_t>
{
public:
    template <typename string_t>
    json::basic_value<string_t> to_json(const std::nullptr_t&) const
    {
        return json::basic_value<string_t> {};
    }

    template <typename string_t>
    bool check_json(const json::basic_value<string_t>& json) const
    {
        return json.is_null();
    }

    template <typename string_t>
    bool from_json(const json::basic_value<string_t>& json, std::nullptr_t&)
    {
        return check_json(json);
    }
};

template <>
class jsonization<std::filesystem::path>
{
    template <typename string_t>
    constexpr static bool is_string_validate =
        std::is_same_v<string_t, std::filesystem::path::string_type>
        || std::is_same_v<string_t, std::string>;

public:
    template <typename string_t>
    json::basic_value<string_t> to_json(const std::filesystem::path& path) const
    {
        if constexpr (std::is_same_v<string_t, std::filesystem::path::string_type>) {
            return path.native();
        }
        else if constexpr (std::is_same_v<string_t, std::string>) {
            return path.string();
        }
        else {
            static_assert(is_string_validate<string_t>, "Unknown string type");
        }
    }

    template <typename string_t>
    bool check_json(const json::basic_value<string_t>& json) const
    {
        return json.is_string();
    }

    template <typename string_t>
    bool from_json(const json::basic_value<string_t>& json, std::filesystem::path& path) const
    {
        path = json.as_string();
        return true;
    }
};

template <template <typename, size_t> typename arr_t, typename value_t, size_t size>
class jsonization<arr_t<value_t, size>>
    : public __jsonization_array<jsonization<arr_t<value_t, size>>, arr_t<value_t, size>, size>
{
public:
    template <typename string_t>
    json::basic_array<string_t> to_json_array(const arr_t<value_t, size>& value) const
    {
        json::basic_array<string_t> result;
        for (size_t i = 0; i < size; i++) {
            result.emplace_back(value.at(i));
        }
        return result;
    }

    template <typename string_t>
    bool check_json_array(const json::basic_array<string_t>& arr) const
    {
        return arr.template all<value_t>();
    }

    template <typename string_t>
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

template <typename elem1_t, typename elem2_t>
class jsonization<std::pair<elem1_t, elem2_t>>
    : public __jsonization_array<
          jsonization<std::pair<elem1_t, elem2_t>>,
          std::pair<elem1_t, elem2_t>,
          2>
{
public:
    template <typename string_t>
    json::basic_array<string_t> to_json_array(const std::pair<elem1_t, elem2_t>& value) const
    {
        return json::basic_array<string_t> {
            json::basic_value<string_t> { value.first },
            json::basic_value<string_t> { value.second },
        };
    }

    template <typename string_t>
    bool check_json_array(const json::basic_array<string_t>& arr) const
    {
        return arr[0].template is<elem1_t>() && arr[1].template is<elem2_t>();
    }

    template <typename string_t>
    bool from_json_array(const json::basic_array<string_t>& arr, std::pair<elem1_t, elem2_t>& value)
        const
    {
        if (!check_json_array(arr)) {
            return false;
        }
        value.first = arr[0].template as<elem1_t>();
        value.second = arr[1].template as<elem2_t>();
        return true;
    }
};

template <typename... args_t>
class jsonization<std::tuple<args_t...>>
    : public __jsonization_array<
          jsonization<std::tuple<args_t...>>,
          std::tuple<args_t...>,
          std::tuple_size_v<std::tuple<args_t...>>>
{
public:
    using tuple_t = std::tuple<args_t...>;
    constexpr static size_t tuple_size = std::tuple_size_v<tuple_t>;

    template <typename string_t>
    json::basic_array<string_t> to_json_array(const tuple_t& value) const
    {
        json::basic_array<string_t> result;
        to_json_impl(result, value, std::make_index_sequence<tuple_size>());
        return result;
    }

    template <typename string_t, std::size_t... Is>
    void
        to_json_impl(json::basic_array<string_t>& arr, const tuple_t& t, std::index_sequence<Is...>)
            const
    {
        (arr.emplace_back(std::get<Is>(t)), ...);
    }

    template <typename string_t>
    bool check_json_array(const json::basic_array<string_t>& arr) const
    {
        return check_json_impl(arr, std::make_index_sequence<tuple_size>());
    }

    template <typename string_t, std::size_t... Is>
    bool check_json_impl(const json::basic_array<string_t>& arr, std::index_sequence<Is...>) const
    {
        return (arr[Is].template is<std::tuple_element_t<Is, tuple_t>>() && ...);
    }

    template <typename string_t>
    bool from_json_array(const json::basic_array<string_t>& arr, tuple_t& value) const
    {
        if (!check_json_array(arr)) {
            return false;
        }

        from_json_impl(arr, value, std::make_index_sequence<tuple_size>());
        return true;
    }

    template <typename string_t, std::size_t... Is>
    void from_json_impl(
        const json::basic_array<string_t>& arr,
        tuple_t& t,
        std::index_sequence<Is...>) const
    {
        ((std::get<Is>(t) = arr[Is].template as<std::tuple_element_t<Is, tuple_t>>()), ...);
    }
};

template <typename... args_t>
class jsonization<std::variant<args_t...>>
{
public:
    using variant_t = std::variant<args_t...>;
    constexpr static size_t variant_size = std::variant_size_v<variant_t>;

    template <typename string_t>
    json::basic_value<string_t> to_json(const variant_t& value) const
    {
        json::basic_value<string_t> result;
        to_json_impl(result, value, std::make_index_sequence<variant_size>());
        return result;
    }

    template <typename string_t, std::size_t... Is>
    void to_json_impl(
        json::basic_value<string_t>& val,
        const variant_t& t,
        std::index_sequence<Is...>) const
    {
        std::ignore = ((t.index() == Is ? (val = std::get<Is>(t), true) : false) || ...);
    }

    template <typename string_t>
    bool check_json(const json::basic_value<string_t>& json) const
    {
        return check_json_impl(json, std::make_index_sequence<variant_size>());
    }

    template <typename string_t, std::size_t... Is>
    bool check_json_impl(const json::basic_value<string_t>& val, std::index_sequence<Is...>) const
    {
        return (val.template is<std::variant_alternative_t<Is, variant_t>>() || ...);
    }

    template <typename string_t>
    bool from_json(const json::basic_value<string_t>& json, variant_t& value) const
    {
        if (!check_json_impl(json, std::make_index_sequence<variant_size>())) {
            return false;
        }

        from_json_impl(json, value, std::make_index_sequence<variant_size>());
        return true;
    }

    template <typename string_t, std::size_t... Is>
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

}
