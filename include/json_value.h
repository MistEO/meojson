#pragma once

#include <string>
#include <map>
#include <vector>

namespace json
{
    class object;
    class array;

    enum class ValueType
    {
        Invalid,
        WhiteSpace,
        Null,
        Boolean,
        String,
        Number,
        Array,
        Object,
        NUM_T
    };

    class value
    {
    public:
        value() = default;
        value(const value &rhs) = default;
        value(value &&rhs) = default;
        value(const object &obj);
        value(object &&obj);
        value(const array &arr);
        value(array &&arr);

        ~value() = default;

        bool empty() const;

        bool as_boolean() const;
        int as_integer() const;
        double as_double() const;
        std::string as_string() const;
        object as_object() const;
        array as_array() const;

        std::string to_string() const;

        static value string(const char *str);
        static value string(const std::string &str);
        static value number(int num);
        static value number(double num);
        static value boolean(bool b);
        // static value object(const object &obj);
        // static value array(const array &arr);
        static value null();

        value &operator=(const value &) = default;
        value &operator=(value &&) = default;

        void set_raw_basic_data(ValueType type, const std::string &basic_data);
        void set_raw_basic_data(ValueType type, std::string &&basic_data);

    private:
        ValueType _type = ValueType::WhiteSpace;
        std::string _basic_type_data;
        std::map<std::string, value> _object_data;
        std::vector<value> _array_data;
    };
} // namespace json