#pragma once

#include <string>
#include <map>
#include <vector>

#include "json_value_type.h"

namespace json
{
    class object;
    class array;

    class value
    {
    public:
        value() = default;
        value(const value &rhs) = default;
        value(const object &obj);
        value(const array &arr);

        ~value() = default;

        bool empty() const;

        bool as_boolean() const;
        int as_integer() const;
        double as_double() const;
        std::string as_string() const;
        object as_object() const;
        array as_array() const;

        std::string to_string() const;

        static json::value string(const char *str);
        static json::value string(const std::string &str);
        static json::value number(int num);
        static json::value number(double num);
        static json::value boolean(bool b);
        // static json::value object(const json::object &obj);
        // static json::value array(const json::array &arr);
        static json::value null();

        void set_raw_basic_data(json::ValueType type, const std::string &basic_data);

    private:
        json::ValueType _type = ValueType::JsonWhiteSpace;
        std::string _basic_type_data;
        std::map<std::string, json::value> _object_data;
        std::vector<json::value> _array_data;
    };
} // namespace json