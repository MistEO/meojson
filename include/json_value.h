#pragma once

#include <string>
#include <ostream>
#include <memory>
#include <mutex>

namespace json
{
    enum class value_type : char
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

    class array;
    class object;
    class parser;

    class value
    {
        friend class parser;

        using unique_array = std::unique_ptr<array>;
        using unique_object = std::unique_ptr<object>;

    public:
        value();
        value(const value &rhs);
        value(value &&rhs) noexcept;

        value(bool b);

        value(int num);
        value(unsigned num);
        value(long num);
        value(unsigned long num);
        value(long long num);
        value(unsigned long long num);
        value(float num);
        value(double num);
        value(long double num);

        value(const char *str);
        value(const std::string &str);
        value(std::string &&str);

        value(const array &arr);
        value(array &&arr);
        // value(std::initializer_list<value> init_list); // for array

        value(const object &obj);
        value(object &&obj);
        // error: conversion from ‘<brace-enclosed initializer list>’ to ‘json::value’ is ambiguous
        // value(std::initializer_list<std::pair<std::string, value>> init_list); // for object

        ~value();

        bool valid() const noexcept { return _type != value_type::Invalid ? true : false; }
        bool empty() const noexcept { return (_type == value_type::Null && _raw_data.compare("null") == 0) ? true : false; }
        value_type type() const noexcept { return _type; }
        const value &at(size_t pos) const;
        const value &at(const std::string &key) const;

        bool as_boolean() const;
        int as_integer() const;
        // unsigned as_unsigned() const;
        long as_long() const;
        unsigned long as_unsigned_long() const;
        long long as_long_long() const;
        unsigned long long as_unsigned_long_long() const;
        float as_float() const;
        double as_double() const;
        long double as_long_double() const;
        std::string as_string() const;
        array as_array() const;
        object as_object() const;

        // return raw string
        std::string to_string() const;
        std::string format(std::string shift_str = "    ", size_t basic_shift_count = 0) const;

        value &operator=(const value &rhs);
        value &operator=(value &&) noexcept;

        const value &operator[](size_t pos) const;
        value &operator[](size_t pos);
        value &operator[](const std::string &key);
        value &operator[](std::string &&key);

        static value invalid_value();

    private:
        // for parser
        template <typename T>
        value(value_type type, T &&raw_data)
            : _type(type),
              _raw_data((type == value_type::Array || type == value_type::Object) ? std::string() : std::forward<T>(raw_data)),
              _lazy_data((type == value_type::Array || type == value_type::Object) ? std::forward<T>(raw_data) : std::string())
        {
            static_assert(
                std::is_constructible<std::string, T>::value,
                "Parameter n can't be used to construct a std::string");
        }

        void parse_lazy_data() const;

        value_type _type = value_type::Null;
        std::string _raw_data = "null"; // If the value_type is Object or Array, the _raw_data will be a empty string.
        mutable std::string _lazy_data;
        mutable std::mutex _lazy_mutex;
        mutable unique_array _array_ptr;
        mutable unique_object _object_ptr;
    };

    std::ostream &operator<<(std::ostream &out, const value &val);

} // namespace json