#pragma once

#include <string>

namespace json
{
    class object;
    class array;

    class value
    {
    public:
        value() = default;
        value(const value &rhs) = default;

        ~value() = default;

        int as_int() const;
        double as_double() const;
        std::string as_string() const;
        object as_object() const;
        array as_array() const;

        std::string to_string() const;

    private:
        std::string m_raw;
    };
} // namespace json