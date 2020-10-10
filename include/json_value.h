#pragma once

#include <string>
#include <map>

#include "basic_json.h"

namespace json
{
    class object;
    class array;
    class value : public basic_json
    {
    public:
        value() = default;
        value(const value &rhs) = default;

        ~value() = default;

        // bool valid() const override;

        bool as_boolean() const;
        int as_integer() const;
        double as_double() const;
        std::string as_string() const;
        object as_object() const;
        array as_array() const;

        std::string to_string() const override;

        static json::value string(const char *str);
        static json::value string(const std::string &str);
        static json::value number(int num);
        static json::value number(double num);
        static json::value boolean(bool b);
        //static json::value object(const json::object &obj);
        //static json::value array(const json::array &arr);
        static json::value null();
    };
} // namespace json