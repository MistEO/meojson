#pragma once

#include <string>
#include <map>
#include <vector>
#include <ostream>

namespace json
{
    class object;
    class array;

    enum class ValueType
    {
        Invalid,
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
        value(const array &arr);
        value(array &&arr);
        value(const object &obj);
        value(object &&obj);

        ~value() = default;

        bool valid() const;
        bool empty() const;
        ValueType type() const;
        bool as_boolean() const;
        int as_integer() const;
        double as_double() const;
        std::string as_string() const;
        array as_array() const;
        object as_object() const;

        std::string to_string() const;

        void set_raw_basic_data(ValueType type, const std::string &basic_data);
        void set_raw_basic_data(ValueType type, std::string &&basic_data);

        value &operator=(const value &) = default;
        value &operator=(value &&) = default;

        static value null();
        static value boolean(bool b);
        static value number(int num);
        static value number(double num);
        static value string(const char *str);
        static value string(const std::string &str);
        static value string(std::string &&str);

    private:
        ValueType _type = ValueType::Null;
        std::string _basic_type_data = "null";
        std::map<std::string, value> _object_data;
        std::vector<value> _array_data;
    };

    std::ostream &operator<<(std::ostream &out, const value &value);

} // namespace json