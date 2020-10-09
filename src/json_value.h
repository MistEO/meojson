#pragma once

#include <string>

namespace json
{
    class value
    {
    public:
        enum ValueType
        {
            JsonInvalid,
            JsonNull,
            JsonBoolean,
            JsonString,
            JsonNumber,
            JsonArray,
            JsonObject
        };

        value() = default;
        value(const value &rhs) = default;

        ~value() = default;

        bool parse(const std::string &content);
        bool valid() const;
        ValueType type() const;

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
        //static json::value object(const json::object &obj);
        //static json::value array(const json::array &arr);
        static json::value null();

    protected:
        static const std::string reg_str_json_whitespace;

        static const std::string reg_str_json_null;
        static const std::string reg_str_json_boolean;
        static const std::string reg_str_json_string;
        static const std::string reg_str_json_number;

        static const std::string reg_str_json_non_nested_value;

        static const std::regex reg_json_null;
        static const std::regex reg_json_boolean;
        static const std::regex reg_json_string;
        static const std::regex reg_json_number;
        static const std::regex reg_json_non_nested_value;

    private:
        static const std::string reg_str_json_number_fraction;
        static const std::string reg_str_json_number_exponent;

        std::string::const_iterator parse_once(const std::string &content, const std::string::const_iterator &first);

        std::string m_raw;
        ValueType m_type;
    };
} // namespace json