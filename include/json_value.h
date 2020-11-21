#pragma once

#include <string>
#include <ostream>
#include <memory>

namespace json
{
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

    class array;
    class object;
    class parser;

    struct array_deleter final
    {
        void operator()(array *p);
    };
    struct object_deleter final
    {
        void operator()(object *p);
    };

    using unique_array = std::unique_ptr<array, array_deleter>;
    using unique_object = std::unique_ptr<object, object_deleter>;

    class value
    {
        friend class parser;

    public:
        value() = default;
        value(const value &rhs);
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
        array as_array();
        object as_object();

        // return raw string
        std::string to_string() const;

        value &operator=(const value &) = default;
        value &operator=(value &&) = default;

        // const value &operator[](size_t pos) const;
        value &operator[](size_t pos);
        value &operator[](const std::string &key);
        value &operator[](std::string &&key);

        static value invalid_value();

    private:
        // for parser
        value(value_type type, std::string &&raw_data);
        value(unique_array &&arr_ptr);
        value(unique_object &&obj_ptr);

        value_type _type = value_type::Null;
        std::string _raw_data = "null"; // In non-lazy parsing, if the value_type is Object or Array, the _raw_data will be empty string.
        unique_array _array_ptr = nullptr;
        unique_object _object_ptr = nullptr;
    };

    std::ostream &operator<<(std::ostream &out, const value &val);

} // namespace json