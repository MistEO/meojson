// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <optional>

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

}
