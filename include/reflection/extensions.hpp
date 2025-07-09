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
        return value.has_value() ? json::value {} : json::value(value.value());
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
        try {
            value.value() = value_t(j);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }
};

template <typename value_t>
class jsonization<meo_opt<value_t>>
{
public:
    json::value to_json(const meo_opt<value_t>& value) const
    {
        return value.has_value() ? json::value {} : json::value(value.value());
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
        try {
            value.value() = value_t(j);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }
};

}
