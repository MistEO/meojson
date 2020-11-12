#pragma once

#include <string>
#include <ostream>

#include "json_object.h"
#include "json_array.h"

namespace json
{
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
        value(value &&rhs) noexcept = default;
        value(const array &arr);
        value(array &&arr) noexcept;
        value(const object &obj);
        value(object &&obj);

        ~value() = default;

        bool valid() const noexcept;
        bool empty() const noexcept;
        ValueType type() const noexcept;
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
        object _object;
        array _array;
    };

    std::ostream &operator<<(std::ostream &out, const value &val);

} // namespace json