#pragma once

#include <string>
#include <ostream>
#include <memory>

namespace json
{
    class array;
    class object;

    enum class value_type
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

        ~value() = default;

        bool valid() const noexcept;
        bool empty() const noexcept;
        value_type type() const noexcept;
        const value &at(size_t pos) const;
        const value &at(const std::string key) const;

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

        void set_raw_basic_data(value_type type, const std::string &basic_data);
        void set_raw_basic_data(value_type type, std::string &&basic_data);

        value &operator=(const value &) = default;
        value &operator=(value &&) = default;

        const value &operator[](size_t pos) const;
        value &operator[](size_t pos);
        value &operator[](const std::string &key);
        value &operator[](std::string &&key);

        static value invalid_value();

    private:
        value_type _type = value_type::Null;
        std::string _raw_basic_data = "null";
        std::shared_ptr<array> _array_ptr = nullptr;
        std::shared_ptr<object> _object_ptr = nullptr;
    };

    std::ostream &operator<<(std::ostream &out, const value &val);

} // namespace json