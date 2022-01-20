#pragma once

#include <initializer_list>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "common.h"

namespace json
{
    // *************************
    // *   exception declare   *
    // *************************
    class exception : public std::exception
    {
    public:
        exception() = default;
        exception(const std::string& msg) : m_msg(msg) {}

        exception(const exception&) = default;
        exception& operator=(const exception&) = default;
        exception(exception&&) = default;
        exception& operator=(exception&&) = default;

        virtual ~exception() noexcept override = default;

        virtual const char* what() const noexcept override { return m_msg.c_str(); }

    private:
        std::string m_msg;
    };

    class array;
    class object;

    // *************************
    // *     value declare     *
    // *************************
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

    class value
    {
        using unique_array = std::unique_ptr<array>;
        using unique_object = std::unique_ptr<object>;

    public:
        value();
        value(const value& rhs);
        value(value&& rhs) noexcept;

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

        value(const char* str);
        value(const std::string& str);
        value(std::string&& str);

        value(const array& arr);
        value(array&& arr);
        // value(std::initializer_list<value> init_list); // for array

        value(const object& obj);
        value(object&& obj);
        // error: conversion from ‘<brace-enclosed initializer list>’ to ‘value’
        // is ambiguous value(std::initializer_list<std::pair<std::string, value>>
        // init_list); // for object

        // Constructed from raw data
        template <typename... Args> value(value_type type, Args &&...args);

        // Prohibit conversion of other types to value
        template <typename T> value(T) = delete;

        ~value();

        bool valid() const noexcept
        {
            return _type != value_type::Invalid ? true : false;
        }
        bool empty() const noexcept
        {
            return (_type == value_type::Null && _raw_data.compare("null") == 0)
                ? true
                : false;
        }
        bool is_null() const noexcept { return empty(); }
        bool is_number() const noexcept { return _type == value_type::Number; }
        bool is_boolean() const noexcept { return _type == value_type::Boolean; }
        bool is_string() const noexcept { return _type == value_type::String; }
        bool is_array() const noexcept { return _type == value_type::Array; }
        bool is_object() const noexcept { return _type == value_type::Object; }
        bool exist(const std::string& key) const;
        bool exist(size_t pos) const;
        value_type type() const noexcept { return _type; }
        const value& at(size_t pos) const;
        const value& at(const std::string& key) const;

        template <typename Type>
        Type get(const std::string& key, const Type& default_value) const
        {
            return is_object() ? as_object().get(key, default_value) : default_value;
        }

        template <typename Type>
        Type get(size_t pos, const Type& default_value) const
        {
            return is_array() ? as_array().get(pos, default_value) : default_value;
        }

        const bool as_boolean() const;
        const int as_integer() const;
        // const unsigned as_unsigned() const;
        const long as_long() const;
        const unsigned long as_unsigned_long() const;
        const long long as_long_long() const;
        const unsigned long long as_unsigned_long_long() const;
        const float as_float() const;
        const double as_double() const;
        const long double as_long_double() const;
        const std::string as_string() const;
        const array& as_array() const;
        const object& as_object() const;

        array& as_array();
        object& as_object();

        // return raw string
        const std::string to_string() const;
        const std::string format(std::string shift_str = "    ",
                                 size_t basic_shift_count = 0) const;

        value& operator=(const value& rhs);
        value& operator=(value&&) noexcept;

        const value& operator[](size_t pos) const;
        value& operator[](size_t pos);
        value& operator[](const std::string& key);
        value& operator[](std::string&& key);
        // explicit operator bool() const noexcept { return valid(); }

        explicit operator bool() const { return as_boolean(); }
        explicit operator int() const { return as_integer(); }
        explicit operator long() const { return as_long(); }
        explicit operator unsigned long() const { return as_unsigned_long(); }
        explicit operator long long() const { return as_long_long(); }
        explicit operator unsigned long long() const
        {
            return as_unsigned_long_long();
        }
        explicit operator float() const { return as_float(); }
        explicit operator double() const { return as_double(); }
        explicit operator long double() const { return as_long_double(); }
        explicit operator std::string() const { return as_string(); }

    private:
        template <typename T>
        static std::unique_ptr<T> copy_unique_ptr(const std::unique_ptr<T>& t)
        {
            return t == nullptr ? nullptr : std::make_unique<T>(*t);
        }

        value_type _type = value_type::Null;
        std::string _raw_data = "null"; // If the value_type is Object or Array, the
        // _raw_data will be a empty string.
        unique_array _array_ptr;
        unique_object _object_ptr;
    };

    const value invalid_value();

    // *************************
    // *     array declare     *
    // *************************
    class array
    {
    public:
        using raw_array = std::vector<value>;
        using iterator = raw_array::iterator;
        using const_iterator = raw_array::const_iterator;
        using reverse_iterator = raw_array::reverse_iterator;
        using const_reverse_iterator = raw_array::const_reverse_iterator;

        array() = default;
        array(const array& rhs) = default;
        array(array&& rhs) noexcept = default;
        array(const raw_array& arr);
        array(raw_array&& arr) noexcept;
        array(std::initializer_list<raw_array::value_type> init_list);

        template <typename ArrayType> array(ArrayType arr);

        ~array() noexcept = default;

        bool empty() const noexcept { return _array_data.empty(); }
        size_t size() const noexcept { return _array_data.size(); }
        bool exist(size_t pos) const { return _array_data.size() < pos; }
        const value& at(size_t pos) const;
        const std::string to_string() const;
        const std::string format(std::string shift_str = "    ",
                                 size_t basic_shift_count = 0) const;

        const bool get(size_t pos, bool default_value) const;
        const int get(size_t pos, int default_value) const;
        const long get(size_t pos, long default_value) const;
        const unsigned long get(size_t pos, unsigned default_value) const;
        const long long get(size_t pos, long long default_value) const;
        const unsigned long long get(size_t pos,
                                     unsigned long long default_value) const;
        const float get(size_t pos, float default_value) const;
        const double get(size_t pos, double default_value) const;
        const long double get(size_t pos, long double default_value) const;
        const std::string get(size_t pos, std::string default_value) const;
        const std::string get(size_t pos, const char* default_value) const;

        template <typename... Args> decltype(auto) emplace_back(Args &&...args);

        void clear() noexcept;
        // void earse(size_t pos);

        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        reverse_iterator rbegin() noexcept;
        reverse_iterator rend() noexcept;
        const_reverse_iterator rbegin() const noexcept;
        const_reverse_iterator rend() const noexcept;
        const_reverse_iterator crbegin() const noexcept;
        const_reverse_iterator crend() const noexcept;

        const value& operator[](size_t pos) const;
        value& operator[](size_t pos);

        array& operator=(const array&) = default;
        array& operator=(array&&) noexcept = default;

        // const raw_array &raw_data() const;

    private:
        raw_array _array_data;
    };

    // *************************
    // *     object declare    *
    // *************************
    class object
    {
    public:
        using raw_object = std::unordered_map<std::string, value>;
        using iterator = raw_object::iterator;
        using const_iterator = raw_object::const_iterator;

        object() = default;
        object(const object& rhs) = default;
        object(object&& rhs) = default;
        object(const raw_object& raw_obj);
        object(raw_object&& raw_obj);
        object(std::initializer_list<raw_object::value_type> init_list);
        template <typename MapType> object(MapType map);

        ~object() = default;

        bool empty() const noexcept { return _object_data.empty(); }
        size_t size() const noexcept { return _object_data.size(); }
        bool exist(const std::string& key) const
        {
            return _object_data.find(key) != _object_data.cend();
        }
        const value& at(const std::string& key) const;
        const std::string to_string() const;
        const std::string format(std::string shift_str = "    ",
                                 size_t basic_shift_count = 0) const;

        const bool get(const std::string& key, bool default_value) const;
        const int get(const std::string& key, int default_value) const;
        const long get(const std::string& key, long default_value) const;
        const unsigned long get(const std::string& key, unsigned default_value) const;
        const long long get(const std::string& key, long long default_value) const;
        const unsigned long long get(const std::string& key,
                                     unsigned long long default_value) const;
        const float get(const std::string& key, float default_value) const;
        const double get(const std::string& key, double default_value) const;
        const long double get(const std::string& key,
                              long double default_value) const;
        const std::string get(const std::string& key,
                              std::string default_value) const;
        const std::string get(const std::string& key,
                              const char* default_value) const;

        template <typename... Args> decltype(auto) emplace(Args &&...args);

        void clear() noexcept;
        bool earse(const std::string& key);

        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        value& operator[](const std::string& key);
        value& operator[](std::string&& key);

        object& operator=(const object&) = default;
        object& operator=(object&&) = default;

        // const raw_object &raw_data() const;

    private:
        raw_object _object_data;
    };

    // *************************
    // *      aux declare      *
    // *************************

    inline std::string unescape_string(std::string&& str)
    {
        std::string replace_str;
        std::string escape_str = std::move(str);

        for (size_t pos = 0; pos < escape_str.size(); ++pos) {
            switch (escape_str[pos]) {
            case '\"':
                replace_str = R"(\")";
                break;
            case '\\':
                replace_str = R"(\\)";
                break;
            case '\b':
                replace_str = R"(\b)";
                break;
            case '\f':
                replace_str = R"(\f)";
                break;
            case '\n':
                replace_str = R"(\n)";
                break;
            case '\r':
                replace_str = R"(\r)";
                break;
            case '\t':
                replace_str = R"(\t)";
                break;
            default:
                continue;
                break;
            }
            escape_str.replace(pos, 1, replace_str);
            ++pos;
        }
        return escape_str;
    }

    inline std::string unescape_string(const std::string& str)
    {
        return unescape_string(std::string(str));
    }

    inline std::string escape_string(std::string&& str)
    {
        std::string escape_str = std::move(str);

        for (size_t pos = 0; pos + 1 < escape_str.size(); ++pos) {
            if (escape_str[pos] != '\\') {
                continue;
            }
            std::string replace_str;
            switch (escape_str[pos + 1]) {
            case '"':
                replace_str = "\"";
                break;
            case '\\':
                replace_str = "\\";
                break;
            case 'b':
                replace_str = "\b";
                break;
            case 'f':
                replace_str = "\f";
                break;
            case 'n':
                replace_str = "\n";
                break;
            case 'r':
                replace_str = "\r";
                break;
            case 't':
                replace_str = "\t";
                break;
            default:
                return std::string();
                break;
            }
            escape_str.replace(pos, 2, replace_str);
        }
        return escape_str;
    }

    inline std::string escape_string(const std::string& str)
    {
        return escape_string(std::string(str));
    }

    enum class value_type : char;

    // *************************
    // *       array impl      *
    // *************************
    template <typename... Args> decltype(auto) array::emplace_back(Args &&...args)
    {
        static_assert(std::is_constructible<raw_array::value_type, Args...>::value,
                      "Parameter can't be used to construct a raw_array::value_type");
        return _array_data.emplace_back(std::forward<Args>(args)...);
    }

    MEOJSON_INLINE array::array(const raw_array& arr) : _array_data(arr) { ; }

    MEOJSON_INLINE array::array(raw_array&& arr) noexcept
        : _array_data(std::move(arr))
    {
        ;
    }

    MEOJSON_INLINE
        array::array(std::initializer_list<raw_array::value_type> init_list)
        : _array_data(init_list)
    {
        ;
    }

    MEOJSON_INLINE const value& array::at(size_t pos) const
    {
        return _array_data.at(pos);
    }

    MEOJSON_INLINE void array::clear() noexcept { _array_data.clear(); }

    MEOJSON_INLINE const std::string array::to_string() const
    {
        std::string str = "[";
        for (const value& val : _array_data) {
            str += val.to_string() + ",";
        }
        if (str.back() == ',') {
            str.pop_back();
        }
        str += "]";
        return str;
    }

    MEOJSON_INLINE const std::string array::format(std::string shift_str,
                                                   size_t basic_shift_count) const
    {
        std::string shift;
        for (size_t i = 0; i != basic_shift_count + 1; ++i) {
            shift += shift_str;
        }

        std::string str = "[";
        for (const value& val : _array_data) {
            str += "\n" + shift + val.format(shift_str, basic_shift_count + 1) + ",";
        }
        if (str.back() == ',') {
            str.pop_back(); // pop last ','
        }

        str += '\n';
        for (size_t i = 0; i != basic_shift_count; ++i) {
            str += shift_str;
        }
        str += ']';
        return str;
    }

    MEOJSON_INLINE const bool array::get(size_t pos, bool default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_boolean()) {
                return value.as_boolean();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const int array::get(size_t pos, int default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_integer();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const long array::get(size_t pos, long default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const unsigned long array::get(size_t pos,
                                                  unsigned default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_unsigned_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const long long array::get(size_t pos,
                                              long long default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const unsigned long long
        array::get(size_t pos, unsigned long long default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_unsigned_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const float array::get(size_t pos, float default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_float();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const double array::get(size_t pos, double default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const long double array::get(size_t pos,
                                                long double default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_number()) {
                return value.as_long_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const std::string array::get(size_t pos,
                                                std::string default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_string()) {
                return value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const std::string array::get(size_t pos,
                                                const char* default_value) const
    {
        if (exist(pos)) {
            value value = _array_data.at(pos);
            if (value.is_string()) {
                return value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE array::iterator array::begin() noexcept
    {
        return _array_data.begin();
    }

    MEOJSON_INLINE array::iterator array::end() noexcept
    {
        return _array_data.end();
    }

    MEOJSON_INLINE array::const_iterator array::begin() const noexcept
    {
        return _array_data.begin();
    }

    MEOJSON_INLINE array::const_iterator array::end() const noexcept
    {
        return _array_data.end();
    }

    MEOJSON_INLINE array::const_iterator array::cbegin() const noexcept
    {
        return _array_data.cbegin();
    }

    MEOJSON_INLINE array::const_iterator array::cend() const noexcept
    {
        return _array_data.cend();
    }

    MEOJSON_INLINE array::reverse_iterator array::rbegin() noexcept
    {
        return _array_data.rbegin();
    }

    MEOJSON_INLINE array::reverse_iterator array::rend() noexcept
    {
        return _array_data.rend();
    }

    MEOJSON_INLINE array::const_reverse_iterator array::rbegin() const noexcept
    {
        return _array_data.rbegin();
    }

    MEOJSON_INLINE array::const_reverse_iterator array::rend() const noexcept
    {
        return _array_data.rend();
    }

    MEOJSON_INLINE array::const_reverse_iterator array::crbegin() const noexcept
    {
        return _array_data.crbegin();
    }

    MEOJSON_INLINE array::const_reverse_iterator array::crend() const noexcept
    {
        return _array_data.crend();
    }

    MEOJSON_INLINE value& array::operator[](size_t pos) { return _array_data[pos]; }

    MEOJSON_INLINE const value& array::operator[](size_t pos) const
    {
        return _array_data[pos];
    }

    // const raw_array &array::raw_data() const
    // {
    //     return _array_data;
    // }

    // *************************
    // *      object impl      *
    // *************************
    template <typename... Args> decltype(auto) object::emplace(Args &&...args)
    {
        static_assert(
            std::is_constructible<raw_object::value_type, Args...>::value,
            "Parameter can't be used to construct a raw_object::value_type");
        return _object_data.emplace(std::forward<Args>(args)...);
    }

    MEOJSON_INLINE std::ostream& operator<<(std::ostream& out, const array& arr)
    {
        // TODO: format output

        out << arr.to_string();
        return out;
    }

    MEOJSON_INLINE object::object(const raw_object& raw_obj)
        : _object_data(raw_obj)
    {
        ;
    }

    MEOJSON_INLINE object::object(raw_object&& raw_obj)
        : _object_data(std::move(raw_obj))
    {
        ;
    }

    MEOJSON_INLINE
        object::object(std::initializer_list<raw_object::value_type> init_list)
    {
        for (const auto& [key, val] : init_list) {
            emplace(key, val);
        }
    }

    MEOJSON_INLINE const value& object::at(const std::string& key) const
    {
        return _object_data.at(key);
    }

    MEOJSON_INLINE void object::clear() noexcept { _object_data.clear(); }

    MEOJSON_INLINE bool object::earse(const std::string& key)
    {
        return _object_data.erase(key) > 0 ? true : false;
    }

    MEOJSON_INLINE const std::string object::to_string() const
    {
        std::string str = "{";
        for (const auto& [key, val] : _object_data) {
            str += "\"" + unescape_string(key) + "\":" + val.to_string() + ",";
        }
        if (str.back() == ',') {
            str.pop_back();
        }
        str += "}";
        return str;
    }

    MEOJSON_INLINE const std::string
        object::format(std::string shift_str, size_t basic_shift_count) const
    {
        std::string shift;
        for (size_t i = 0; i != basic_shift_count + 1; ++i) {
            shift += shift_str;
        }

        std::string str = "{";
        for (const auto& [key, val] : _object_data) {
            str += "\n" + shift + "\"" + unescape_string(key) +
                "\": " + val.format(shift_str, basic_shift_count + 1) + ",";
        }
        if (str.back() == ',') {
            str.pop_back(); // pop last ','
        }

        str += '\n';
        for (size_t i = 0; i != basic_shift_count; ++i) {
            str += shift_str;
        }
        str += '}';
        return str;
    }

    MEOJSON_INLINE const bool object::get(const std::string& key,
                                          bool default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_boolean()) {
                return value.as_boolean();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const int object::get(const std::string& key,
                                         int default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_integer();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const long object::get(const std::string& key,
                                          long default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const unsigned long object::get(const std::string& key,
                                                   unsigned default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_unsigned_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const long long object::get(const std::string& key,
                                               long long default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const unsigned long long
        object::get(const std::string& key, unsigned long long default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_unsigned_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const float object::get(const std::string& key,
                                           float default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_float();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const double object::get(const std::string& key,
                                            double default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const long double object::get(const std::string& key,
                                                 long double default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_number()) {
                return value.as_long_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const std::string object::get(const std::string& key,
                                                 std::string default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_string()) {
                return value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE const std::string object::get(const std::string& key,
                                                 const char* default_value) const
    {
        if (exist(key)) {
            value value = _object_data.at(key);
            if (value.is_string()) {
                return value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    MEOJSON_INLINE object::iterator object::begin() noexcept
    {
        return _object_data.begin();
    }

    MEOJSON_INLINE object::iterator object::end() noexcept
    {
        return _object_data.end();
    }

    MEOJSON_INLINE object::const_iterator object::begin() const noexcept
    {
        return _object_data.begin();
    }

    MEOJSON_INLINE object::const_iterator object::end() const noexcept
    {
        return _object_data.end();
    }

    MEOJSON_INLINE object::const_iterator object::cbegin() const noexcept
    {
        return _object_data.cbegin();
    }

    MEOJSON_INLINE object::const_iterator object::cend() const noexcept
    {
        return _object_data.cend();
    }

    MEOJSON_INLINE value& object::operator[](const std::string& key)
    {
        return _object_data[key];
    }

    MEOJSON_INLINE value& object::operator[](std::string&& key)
    {
        return _object_data[std::move(key)];
    }

    // const raw_object &object::raw_data() const
    // {
    //     return _object_data;
    // }

    MEOJSON_INLINE std::ostream& operator<<(std::ostream& out, const object& obj)
    {
        // TODO: format output

        out << obj.to_string();
        return out;
    }

    template <typename MapType> object::object(MapType map)
    {
        static_assert(std::is_constructible<raw_object::value_type,
                                            typename MapType::value_type>::value,
                      "Parameter can't be used to construct a "
                      "object::raw_object::value_type");
        for (auto&& ele : map) {
            _object_data.emplace(std::move(ele));
        }
    }

    // *************************
    // *       value impl      *
    // *************************
    MEOJSON_INLINE value::value() = default;

    MEOJSON_INLINE value::value(const value& rhs)
        : _type(rhs._type), _raw_data(rhs._raw_data),
        _array_ptr(copy_unique_ptr(rhs._array_ptr)),
        _object_ptr(copy_unique_ptr(rhs._object_ptr))
    {
        ;
    }

    MEOJSON_INLINE value::value(value&& rhs) noexcept = default;

    MEOJSON_INLINE value::value(bool b)
        : _type(value_type::Boolean), _raw_data(b ? "true" : "false")
    {
        ;
    }

    MEOJSON_INLINE value::value(int num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(unsigned num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(unsigned long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(long long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(unsigned long long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(float num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(double num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(long double num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    MEOJSON_INLINE value::value(const char* str)
        : _type(value_type::String), _raw_data(unescape_string(str))
    {
        ;
    }

    MEOJSON_INLINE value::value(const std::string& str)
        : _type(value_type::String), _raw_data(unescape_string(str))
    {
        ;
    }

    MEOJSON_INLINE value::value(std::string&& str)
        : _type(value_type::String), _raw_data(unescape_string(std::move(str)))
    {
        ;
    }

    MEOJSON_INLINE value::value(const array& arr)
        : _type(value_type::Array), _raw_data(std::string()),
        _array_ptr(std::make_unique<array>(arr))
    {
        ;
    }

    MEOJSON_INLINE value::value(array&& arr)
        : _type(value_type::Array), _raw_data(std::string()),
        _array_ptr(std::make_unique<array>(std::move(arr)))
    {
        ;
    }

    MEOJSON_INLINE value::value(const object& obj)
        : _type(value_type::Object), _raw_data(std::string()),
        _object_ptr(std::make_unique<object>(obj))
    {
        ;
    }

    MEOJSON_INLINE value::value(object&& obj)
        : _type(value_type::Object), _raw_data(std::string()),
        _object_ptr(std::make_unique<object>(std::move(obj)))
    {
        ;
    }

    // for Pimpl
    MEOJSON_INLINE value::~value() = default;

    MEOJSON_INLINE bool value::exist(const std::string& key) const
    {
        return _type == value_type::Object && as_object().exist(key);
    }

    MEOJSON_INLINE bool value::exist(size_t pos) const
    {
        return _type == value_type::Array && as_array().exist(pos);
    }

    MEOJSON_INLINE const value& value::at(size_t pos) const
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return _array_ptr->at(pos);
        }

        throw exception("Wrong Type or data empty");
    }

    MEOJSON_INLINE const value& value::at(const std::string& key) const
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return _object_ptr->at(key);
        }

        throw exception("Wrong Type or data empty");
    }

    MEOJSON_INLINE const bool value::as_boolean() const
    {
        if (_type == value_type::Boolean) {
            if (_raw_data == "true") {
                return true;
            }
            else if (_raw_data == "false") {
                return false;
            }
            else {
                throw exception("Unknown Parse Error");
            }
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const int value::as_integer() const
    {
        if (_type == value_type::Number) {
            return std::stoi(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    // const unsigned value::as_unsigned() const
    // {
    //     if (_type == value_type::Number)
    //     {
    //         return std::stou(_raw_data); // not exist
    //     }
    //     else
    //     {
    //         throw exception("Wrong Type");
    //     }
    // }

    MEOJSON_INLINE const long value::as_long() const
    {
        if (_type == value_type::Number) {
            return std::stol(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const unsigned long value::as_unsigned_long() const
    {
        if (_type == value_type::Number) {
            return std::stoul(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const long long value::as_long_long() const
    {
        if (_type == value_type::Number) {
            return std::stoll(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const unsigned long long value::as_unsigned_long_long() const
    {
        if (_type == value_type::Number) {
            return std::stoull(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const float value::as_float() const
    {
        if (_type == value_type::Number) {
            return std::stof(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const double value::as_double() const
    {
        if (_type == value_type::Number) {
            return std::stod(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const long double value::as_long_double() const
    {
        if (_type == value_type::Number) {
            return std::stold(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const std::string value::as_string() const
    {
        if (_type == value_type::String) {
            return escape_string(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    MEOJSON_INLINE const array& value::as_array() const
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return *_array_ptr;
        }

        throw exception("Wrong Type");
    }

    MEOJSON_INLINE const object& value::as_object() const
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return *_object_ptr;
        }

        throw exception("Wrong Type or data empty");
    }

    MEOJSON_INLINE array& value::as_array()
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return *_array_ptr;
        }

        throw exception("Wrong Type");
    }

    MEOJSON_INLINE object& value::as_object()
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return *_object_ptr;
        }

        throw exception("Wrong Type or data empty");
    }

    MEOJSON_INLINE const std::string value::to_string() const
    {
        switch (_type) {
        case value_type::Null:
        case value_type::Boolean:
        case value_type::Number:
            return _raw_data;
        case value_type::String:
            return '"' + _raw_data + '"';
        case value_type::Object:
            return _object_ptr->to_string();
        case value_type::Array:
            return _array_ptr->to_string();
        default:
            throw exception("Unknown Value Type");
        }
    }

    MEOJSON_INLINE const std::string value::format(std::string shift_str,
                                                   size_t basic_shift_count) const
    {
        switch (_type) {
        case value_type::Null:
        case value_type::Boolean:
        case value_type::Number:
            return _raw_data;
        case value_type::String:
            return '"' + _raw_data + '"';
        case value_type::Object:
            return _object_ptr->format(shift_str, basic_shift_count);
        case value_type::Array:
            return _array_ptr->format(shift_str, basic_shift_count);
        default:
            throw exception("Unknown Value Type");
        }
    }

    MEOJSON_INLINE value& value::operator=(const value& rhs)
    {
        _type = rhs._type;
        _raw_data = rhs._raw_data;
        _array_ptr = copy_unique_ptr(rhs._array_ptr);
        _object_ptr = copy_unique_ptr(rhs._object_ptr);

        return *this;
    }

    MEOJSON_INLINE value& value::operator=(value&& rhs) noexcept = default;

    MEOJSON_INLINE const value& value::operator[](size_t pos) const
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return _array_ptr->operator[](pos);
        }
        // Array not support to create by operator[]

        throw exception("Wrong Type");
    }

    MEOJSON_INLINE value& value::operator[](size_t pos)
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return _array_ptr->operator[](pos);
        }
        // Array not support to create by operator[]

        throw exception("Wrong Type");
    }

    MEOJSON_INLINE value& value::operator[](const std::string& key)
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return _object_ptr->operator[](key);
        }
        // Create a new value by operator[]
        else if (_type == value_type::Null) {
            _type = value_type::Object;
            _object_ptr = std::make_unique<object>();
            return _object_ptr->operator[](key);
        }

        throw exception("Wrong Type");
    }

    MEOJSON_INLINE value& value::operator[](std::string&& key)
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return _object_ptr->operator[](std::move(key));
        }
        // Create a new value by operator[]
        else if (_type == value_type::Null) {
            _type = value_type::Object;
            _object_ptr = std::make_unique<object>();
            return _object_ptr->operator[](std::move(key));
        }

        throw exception("Wrong Type");
    }

    MEOJSON_INLINE const value invalid_value()
    {
        return value(value_type::Invalid, std::string());
    }

    MEOJSON_INLINE std::ostream& operator<<(std::ostream& out, const value& val)
    {
        // TODO: format output

        out << val.to_string();
        return out;
    }

    template <typename... Args>
    value::value(value_type type, Args &&...args)
        : _type(type), _raw_data(std::forward<Args>(args)...)
    {
        static_assert(std::is_constructible<std::string, Args...>::value,
                      "Parameter can't be used to construct a std::string");
    }

    // std::istream &operator>>(std::istream &in, value &val)
    // {
    //     return in;
    // }

} // namespace json
