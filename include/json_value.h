#pragma once

#include <string>
#include <regex>

namespace json
{
    class object;
    class array;

    class value
    {
    public:
        enum ValueType
        {
            JsonNull,
            JsonBoolean,
            JsonString,
            JsonNumber,
            JsonArray,
            JsonObject,
            JsonInvalid
        };

        value() = default;
        value(const value &rhs) = default;
        value(const json::object &obj);
        value(const json::array &arr);

        ~value() = default;

        bool parse(const std::string &content);
        bool valid() const;

        int as_integer() const;
        bool as_boolean() const;
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
        //static json::value object(const json::object &obj);
        //static json::value array(const json::array &arr);
        static json::value null();

    private:
        std::string m_raw;
        bool m_valid = true;
        ValueType m_type;

    private:
        static const std::string reg_str_json_whitespace;

        static const std::string reg_str_json_null;

        static const std::string reg_str_json_boolean;

        static const std::string reg_str_json_string;

        static const std::string reg_str_json_number_fraction;
        static const std::string reg_str_json_number_exponent;
        static const std::string reg_str_json_number;

        static const std::string reg_str_json_value;

        static const std::string reg_str_json_object_pair;
        static const std::string reg_str_json_object;

        static const std::string reg_str_json_array_element;
        static const std::string reg_str_json_array;

        static const std::regex reg_json_null;
        static const std::regex reg_json_boolean;
        static const std::regex reg_json_string;
        static const std::regex reg_json_number;
        static const std::regex reg_json_object;
        static const std::regex reg_json_array;
        //static const std::regex reg_json_value;
    };
} // namespace json