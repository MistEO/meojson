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

    struct array_deleter final
    {
        constexpr array_deleter() noexcept = default;
        void operator()(array *p) const;
    };
    struct object_deleter final
    {
        constexpr object_deleter() noexcept = default;
        void operator()(object *p) const;
    };

    using unique_array = std::unique_ptr<array, array_deleter>;
    using unique_object = std::unique_ptr<object, object_deleter>;

    class value
    {
        friend class parser;

    public:
        value() = default;
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
        value(value_type type, std::string &&raw_data);
        value(unique_array &&arr_ptr);
        value(unique_object &&obj_ptr);

        void parse_once() const;

        value_type _type = value_type::Null;
        std::string _raw_data = "null"; // If the value_type is Object or Array, the _raw_data will be a empty string.
        mutable std::string _lazy_data;
        mutable std::mutex _lazy_mutex;
        mutable unique_array _array_ptr = nullptr;
        mutable unique_object _object_ptr = nullptr;
    };

    std::ostream &operator<<(std::ostream &out, const value &val);

} // namespace json