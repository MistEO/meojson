#pragma once

#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef JSON5

#include <cctype>
#include <clocale>
#include <cmath>
#include <regex>
#include <stack>
#include <iomanip>
#include <sstream>

#endif  // JSON5

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
        template <typename... Args>
        value(value_type type, Args &&...args);

        // Prohibit conversion of other types to value
        template <typename T>
        value(T) = delete;

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
        value_type   type() const noexcept { return _type; }
        const value& at(size_t pos) const;
        const value& at(const std::string& key) const;

        template <typename Type>
        Type get(const std::string& key, const Type& default_value) const;

        template <typename Type>
        Type get(size_t pos, const Type& default_value) const;

        const bool as_boolean() const;
        const int  as_integer() const;
        // const unsigned as_unsigned() const;
        const long               as_long() const;
        const unsigned long      as_unsigned_long() const;
        const long long          as_long_long() const;
        const unsigned long long as_unsigned_long_long() const;
        const float              as_float() const;
        const double             as_double() const;
        const long double        as_long_double() const;
        const std::string        as_string() const;
        const array& as_array() const;
        const object& as_object() const;

        array& as_array();
        object& as_object();

        // return raw string
        const std::string to_string() const;
        const std::string format(std::string shift_str = "    ",
                                 size_t      basic_shift_count = 0) const;

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
        explicit operator unsigned long long() const { return as_unsigned_long_long(); }
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

        value_type  _type = value_type::Null;
        std::string _raw_data = "null";  // If the value_type is Object or Array, the
        // _raw_data will be a empty string.
        unique_array  _array_ptr;
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

        template <typename ArrayType>
        array(ArrayType arr);

        ~array() noexcept = default;

        bool              empty() const noexcept { return _array_data.empty(); }
        size_t            size() const noexcept { return _array_data.size(); }
        bool              exist(size_t pos) const { return _array_data.size() < pos; }
        const value& at(size_t pos) const;
        const std::string to_string() const;
        const std::string format(std::string shift_str = "    ",
                                 size_t      basic_shift_count = 0) const;

        const bool               get(size_t pos, bool default_value) const;
        const int                get(size_t pos, int default_value) const;
        const long               get(size_t pos, long default_value) const;
        const unsigned long      get(size_t pos, unsigned default_value) const;
        const long long          get(size_t pos, long long default_value) const;
        const unsigned long long get(size_t             pos,
                                     unsigned long long default_value) const;
        const float              get(size_t pos, float default_value) const;
        const double             get(size_t pos, double default_value) const;
        const long double        get(size_t pos, long double default_value) const;
        const std::string        get(size_t pos, std::string default_value) const;
        const std::string        get(size_t pos, const char* default_value) const;

        template <typename... Args>
        decltype(auto) emplace_back(Args &&...args);

        void clear() noexcept;
        // void earse(size_t pos);

        iterator       begin() noexcept;
        iterator       end() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        reverse_iterator       rbegin() noexcept;
        reverse_iterator       rend() noexcept;
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

    std::ostream& operator<<(std::ostream& out, const array& arr);

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
        template <typename MapType>
        object(MapType map);

        ~object() = default;

        bool   empty() const noexcept { return _object_data.empty(); }
        size_t size() const noexcept { return _object_data.size(); }
        bool   exist(const std::string& key) const
        {
            return _object_data.find(key) != _object_data.cend();
        }
        const value& at(const std::string& key) const;
        const std::string to_string() const;
        const std::string format(std::string shift_str = "    ",
                                 size_t      basic_shift_count = 0) const;

        const bool          get(const std::string& key, bool default_value) const;
        const int           get(const std::string& key, int default_value) const;
        const long          get(const std::string& key, long default_value) const;
        const unsigned long get(const std::string& key, unsigned default_value) const;
        const long long get(const std::string& key, long long default_value) const;
        const unsigned long long get(const std::string& key,
                                     unsigned long long default_value) const;
        const float       get(const std::string& key, float default_value) const;
        const double      get(const std::string& key, double default_value) const;
        const long double get(const std::string& key,
                              long double        default_value) const;
        const std::string get(const std::string& key,
                              std::string        default_value) const;
        const std::string get(const std::string& key,
                              const char* default_value) const;

        template <typename... Args>
        decltype(auto) emplace(Args &&...args);

        void clear() noexcept;
        bool earse(const std::string& key);

        iterator       begin() noexcept;
        iterator       end() noexcept;
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

    std::ostream& operator<<(std::ostream& out, const object& obj);

    // *************************
    // *      aux declare      *
    // *************************

    static std::string unescape_string(std::string&& str)
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

    static std::string unescape_string(const std::string& str)
    {
        return unescape_string(std::string(str));
    }

    static std::string escape_string(std::string&& str)
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

    static std::string escape_string(const std::string& str)
    {
        return escape_string(std::string(str));
    }

    enum class value_type : char;

    // *************************
    // *     parser declare    *
    // *************************
    class parser
    {
    public:
        ~parser() noexcept = default;

        static std::optional<value> parse(const std::string& content);

    private:
        parser(const std::string::const_iterator& cbegin,
               const std::string::const_iterator& cend) noexcept
            : _cur(cbegin), _end(cend)
        {
        }

        std::optional<value> parse();
        value                parse_value();

        value parse_null();
        value parse_boolean();
        value parse_number();
        // parse and return a value whose type is value_type::String
        value parse_string();
        value parse_array();
        value parse_object();

        // parse and return a std::string
        std::optional<std::string> parse_stdstring();

        bool skip_whitespace() noexcept;
        bool skip_digit();

    private:
        std::string::const_iterator _cur;
        std::string::const_iterator _end;
    };

    std::optional<value> parse(const std::string& content);

    // *************************
    // *       array impl      *
    // *************************
    template <typename... Args>
    decltype(auto) array::emplace_back(Args &&...args)
    {
        static_assert(std::is_constructible<raw_array::value_type, Args...>::value,
                      "Parameter can't be used to construct a raw_array::value_type");
        return _array_data.emplace_back(std::forward<Args>(args)...);
    }

    array::array(const raw_array& arr) : _array_data(arr) { ; }

    array::array(raw_array&& arr) noexcept : _array_data(std::move(arr)) { ; }

    array::array(std::initializer_list<raw_array::value_type> init_list)
        : _array_data(init_list)
    {
        ;
    }

    const value& array::at(size_t pos) const { return _array_data.at(pos); }

    void array::clear() noexcept { _array_data.clear(); }

    const std::string array::to_string() const
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

    const std::string array::format(std::string shift_str,
                                    size_t      basic_shift_count) const
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
            str.pop_back();  // pop last ','
        }

        str += '\n';
        for (size_t i = 0; i != basic_shift_count; ++i) {
            str += shift_str;
        }
        str += ']';
        return str;
    }

    const bool array::get(size_t pos, bool default_value) const
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

    const int array::get(size_t pos, int default_value) const
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

    const long array::get(size_t pos, long default_value) const
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

    const unsigned long array::get(size_t pos, unsigned default_value) const
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

    const long long array::get(size_t pos, long long default_value) const
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

    const unsigned long long array::get(size_t             pos,
                                        unsigned long long default_value) const
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

    const float array::get(size_t pos, float default_value) const
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

    const double array::get(size_t pos, double default_value) const
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

    const long double array::get(size_t pos, long double default_value) const
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

    const std::string array::get(size_t pos, std::string default_value) const
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

    const std::string array::get(size_t pos, const char* default_value) const
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

    array::iterator array::begin() noexcept { return _array_data.begin(); }

    array::iterator array::end() noexcept { return _array_data.end(); }

    array::const_iterator array::begin() const noexcept
    {
        return _array_data.begin();
    }

    array::const_iterator array::end() const noexcept { return _array_data.end(); }

    array::const_iterator array::cbegin() const noexcept
    {
        return _array_data.cbegin();
    }

    array::const_iterator array::cend() const noexcept
    {
        return _array_data.cend();
    }

    array::reverse_iterator array::rbegin() noexcept
    {
        return _array_data.rbegin();
    }

    array::reverse_iterator array::rend() noexcept { return _array_data.rend(); }

    array::const_reverse_iterator array::rbegin() const noexcept
    {
        return _array_data.rbegin();
    }

    array::const_reverse_iterator array::rend() const noexcept
    {
        return _array_data.rend();
    }

    array::const_reverse_iterator array::crbegin() const noexcept
    {
        return _array_data.crbegin();
    }

    array::const_reverse_iterator array::crend() const noexcept
    {
        return _array_data.crend();
    }

    value& array::operator[](size_t pos) { return _array_data[pos]; }

    const value& array::operator[](size_t pos) const { return _array_data[pos]; }

    // const raw_array &array::raw_data() const
    // {
    //     return _array_data;
    // }

    // *************************
    // *      object impl      *
    // *************************
    template <typename... Args>
    decltype(auto) object::emplace(Args &&...args)
    {
        static_assert(
            std::is_constructible<raw_object::value_type, Args...>::value,
            "Parameter can't be used to construct a raw_object::value_type");
        return _object_data.emplace(std::forward<Args>(args)...);
    }

    std::ostream& operator<<(std::ostream& out, const array& arr)
    {
        // TODO: format output

        out << arr.to_string();
        return out;
    }

    object::object(const raw_object& raw_obj) : _object_data(raw_obj) { ; }

    object::object(raw_object&& raw_obj) : _object_data(std::move(raw_obj)) { ; }

    object::object(std::initializer_list<raw_object::value_type> init_list)
    {
        for (const auto& [key, val] : init_list) {
            emplace(key, val);
        }
    }

    const value& object::at(const std::string& key) const
    {
        return _object_data.at(key);
    }

    void object::clear() noexcept { _object_data.clear(); }

    bool object::earse(const std::string& key)
    {
        return _object_data.erase(key) > 0 ? true : false;
    }

    const std::string object::to_string() const
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

    const std::string object::format(std::string shift_str,
                                     size_t      basic_shift_count) const
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
            str.pop_back();  // pop last ','
        }

        str += '\n';
        for (size_t i = 0; i != basic_shift_count; ++i) {
            str += shift_str;
        }
        str += '}';
        return str;
    }

    const bool object::get(const std::string& key, bool default_value) const
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

    const int object::get(const std::string& key, int default_value) const
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

    const long object::get(const std::string& key, long default_value) const
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

    const unsigned long object::get(const std::string& key,
                                    unsigned           default_value) const
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

    const long long object::get(const std::string& key,
                                long long          default_value) const
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

    const unsigned long long object::get(const std::string& key,
                                         unsigned long long default_value) const
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

    const float object::get(const std::string& key, float default_value) const
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

    const double object::get(const std::string& key, double default_value) const
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

    const long double object::get(const std::string& key,
                                  long double        default_value) const
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

    const std::string object::get(const std::string& key,
                                  std::string        default_value) const
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

    const std::string object::get(const std::string& key,
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

    object::iterator object::begin() noexcept { return _object_data.begin(); }

    object::iterator object::end() noexcept { return _object_data.end(); }

    object::const_iterator object::begin() const noexcept
    {
        return _object_data.begin();
    }

    object::const_iterator object::end() const noexcept
    {
        return _object_data.end();
    }

    object::const_iterator object::cbegin() const noexcept
    {
        return _object_data.cbegin();
    }

    object::const_iterator object::cend() const noexcept
    {
        return _object_data.cend();
    }

    value& object::operator[](const std::string& key) { return _object_data[key]; }

    value& object::operator[](std::string&& key)
    {
        return _object_data[std::move(key)];
    }

    // const raw_object &object::raw_data() const
    // {
    //     return _object_data;
    // }

    std::ostream& operator<<(std::ostream& out, const object& obj)
    {
        // TODO: format output

        out << obj.to_string();
        return out;
    }

    template <typename MapType>
    object::object(MapType map)
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
    value::value() = default;

    value::value(const value& rhs)
        : _type(rhs._type),
        _raw_data(rhs._raw_data),
        _array_ptr(copy_unique_ptr(rhs._array_ptr)),
        _object_ptr(copy_unique_ptr(rhs._object_ptr))
    {
        ;
    }

    value::value(value&& rhs) noexcept = default;

    value::value(bool b)
        : _type(value_type::Boolean), _raw_data(b ? "true" : "false")
    {
        ;
    }

    value::value(int num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(unsigned num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(unsigned long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(long long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(unsigned long long num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(float num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(double num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(long double num)
        : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    value::value(const char* str)
        : _type(value_type::String), _raw_data(unescape_string(str))
    {
        ;
    }

    value::value(const std::string& str)
        : _type(value_type::String), _raw_data(unescape_string(str))
    {
        ;
    }

    value::value(std::string&& str)
        : _type(value_type::String), _raw_data(unescape_string(std::move(str)))
    {
        ;
    }

    value::value(const array& arr)
        : _type(value_type::Array),
        _raw_data(std::string()),
        _array_ptr(std::make_unique<array>(arr))
    {
        ;
    }

    value::value(array&& arr)
        : _type(value_type::Array),
        _raw_data(std::string()),
        _array_ptr(std::make_unique<array>(std::move(arr)))
    {
        ;
    }

    value::value(const object& obj)
        : _type(value_type::Object),
        _raw_data(std::string()),
        _object_ptr(std::make_unique<object>(obj))
    {
        ;
    }

    value::value(object&& obj)
        : _type(value_type::Object),
        _raw_data(std::string()),
        _object_ptr(std::make_unique<object>(std::move(obj)))
    {
        ;
    }

    // for Pimpl
    value::~value() = default;

    bool value::exist(const std::string& key) const
    {
        return _type == value_type::Object && as_object().exist(key);
    }

    bool value::exist(size_t pos) const
    {
        return _type == value_type::Array && as_array().exist(pos);
    }

    const value& value::at(size_t pos) const
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return _array_ptr->at(pos);
        }

        throw exception("Wrong Type or data empty");
    }

    const value& value::at(const std::string& key) const
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return _object_ptr->at(key);
        }

        throw exception("Wrong Type or data empty");
    }

    const bool value::as_boolean() const
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

    const int value::as_integer() const
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

    const long value::as_long() const
    {
        if (_type == value_type::Number) {
            return std::stol(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const unsigned long value::as_unsigned_long() const
    {
        if (_type == value_type::Number) {
            return std::stoul(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const long long value::as_long_long() const
    {
        if (_type == value_type::Number) {
            return std::stoll(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const unsigned long long value::as_unsigned_long_long() const
    {
        if (_type == value_type::Number) {
            return std::stoull(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const float value::as_float() const
    {
        if (_type == value_type::Number) {
            return std::stof(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const double value::as_double() const
    {
        if (_type == value_type::Number) {
            return std::stod(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const long double value::as_long_double() const
    {
        if (_type == value_type::Number) {
            return std::stold(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const std::string value::as_string() const
    {
        if (_type == value_type::String) {
            return escape_string(_raw_data);
        }
        else {
            throw exception("Wrong Type");
        }
    }

    const array& value::as_array() const
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return *_array_ptr;
        }

        throw exception("Wrong Type");
    }

    const object& value::as_object() const
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return *_object_ptr;
        }

        throw exception("Wrong Type or data empty");
    }

    array& value::as_array()
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return *_array_ptr;
        }

        throw exception("Wrong Type");
    }

    object& value::as_object()
    {
        if (_type == value_type::Object && _object_ptr != nullptr) {
            return *_object_ptr;
        }

        throw exception("Wrong Type or data empty");
    }

    const std::string value::to_string() const
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

    const std::string value::format(std::string shift_str,
                                    size_t      basic_shift_count) const
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

    value& value::operator=(const value& rhs)
    {
        _type = rhs._type;
        _raw_data = rhs._raw_data;
        _array_ptr = copy_unique_ptr(rhs._array_ptr);
        _object_ptr = copy_unique_ptr(rhs._object_ptr);

        return *this;
    }

    value& value::operator=(value&& rhs) noexcept = default;

    const value& value::operator[](size_t pos) const
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return _array_ptr->operator[](pos);
        }
        // Array not support to create by operator[]

        throw exception("Wrong Type");
    }

    value& value::operator[](size_t pos)
    {
        if (_type == value_type::Array && _array_ptr != nullptr) {
            return _array_ptr->operator[](pos);
        }
        // Array not support to create by operator[]

        throw exception("Wrong Type");
    }

    value& value::operator[](const std::string& key)
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

    value& value::operator[](std::string&& key)
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

    const value invalid_value()
    {
        return value(value_type::Invalid, std::string());
    }

    std::ostream& operator<<(std::ostream& out, const value& val)
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

    // *************************
    // *      parser impl      *
    // *************************
    std::optional<value> parser::parse(const std::string& content)
    {
        return parser(content.cbegin(), content.cend()).parse();
    }

    std::optional<value> parser::parse()
    {
        if (!skip_whitespace()) {
            return std::nullopt;
        }

        value result_value;
        switch (*_cur) {
        case '[':
            result_value = parse_array();
            break;
        case '{':
            result_value = parse_object();
            break;
        default:  // A JSON payload should be an object or array
            return std::nullopt;
        }

        if (!result_value.valid()) {
            return std::nullopt;
        }

        // After the parsing is complete, there should be no more content other than
        // spaces behind
        if (skip_whitespace()) {
            return std::nullopt;
        }

        return result_value;
    }

    value parser::parse_value()
    {
        switch (*_cur) {
        case 'n':
            return parse_null();
        case 't':
        case 'f':
            return parse_boolean();
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return parse_number();
        case '"':
            return parse_string();
        case '[':
            return parse_array();
        case '{':
            return parse_object();
        default:
            return invalid_value();
        }
    }

    value parser::parse_null()
    {
        static const std::string null_string = "null";

        for (const char& ch : null_string) {
            if (*_cur == ch) {
                ++_cur;
            }
            else {
                return invalid_value();
            }
        }

        return value();
    }

    value parser::parse_boolean()
    {
        static const std::string true_string = "true";
        static const std::string false_string = "false";

        switch (*_cur) {
        case 't':
            for (const char& ch : true_string) {
                if (*_cur == ch) {
                    ++_cur;
                }
                else {
                    return invalid_value();
                }
            }
            return true;
        case 'f':
            for (const char& ch : false_string) {
                if (*_cur == ch) {
                    ++_cur;
                }
                else {
                    return invalid_value();
                }
            }
            return false;
        default:
            return invalid_value();
        }
    }

    value parser::parse_number()
    {
        const auto first = _cur;
        if (*_cur == '-') {
            ++_cur;
        }

        // Numbers cannot have leading zeroes
        if (_cur != _end && *_cur == '0' && _cur + 1 != _end &&
            std::isdigit(*(_cur + 1))) {
            return invalid_value();
        }

        if (!skip_digit()) {
            return invalid_value();
        }

        if (*_cur == '.') {
            ++_cur;
            if (!skip_digit()) {
                return invalid_value();
            }
        }

        if (*_cur == 'e' || *_cur == 'E') {
            if (++_cur == _end) {
                return invalid_value();
            }
            if (*_cur == '+' || *_cur == '-') {
                ++_cur;
            }
            if (!skip_digit()) {
                return invalid_value();
            }
        }

        return value(value_type::Number, first, _cur);
    }

    value parser::parse_string()
    {
        auto string_opt = parse_stdstring();
        if (!string_opt) {
            return invalid_value();
        }
        return value(value_type::String, std::move(string_opt).value());
    }

    value parser::parse_array()
    {
        if (*_cur == '[') {
            ++_cur;
        }
        else {
            return invalid_value();
        }

        if (!skip_whitespace()) {
            return invalid_value();
        }
        else if (*_cur == ']') {
            ++_cur;
            // empty array
            return array();
        }

        array result;
        while (true) {
            if (!skip_whitespace()) {
                return invalid_value();
            }

            value val = parse_value();

            if (!val.valid() || !skip_whitespace()) {
                return invalid_value();
            }

            result.emplace_back(std::move(val));

            if (*_cur == ',') {
                ++_cur;
            }
            else {
                break;
            }
        }

        if (skip_whitespace() && *_cur == ']') {
            ++_cur;
        }
        else {
            return invalid_value();
        }

        return result;
    }

    value parser::parse_object()
    {
        if (*_cur == '{') {
            ++_cur;
        }
        else {
            return invalid_value();
        }

        if (!skip_whitespace()) {
            return invalid_value();
        }
        else if (*_cur == '}') {
            ++_cur;
            // empty object
            return object();
        }

        object result;
        while (true) {
            if (!skip_whitespace()) {
                return invalid_value();
            }

            auto key_opt = parse_stdstring();

            if (key_opt && skip_whitespace() && *_cur == ':') {
                ++_cur;
            }
            else {
                return invalid_value();
            }

            if (!skip_whitespace()) {
                return invalid_value();
            }

            value val = parse_value();

            if (!val.valid() || !skip_whitespace()) {
                return invalid_value();
            }

            std::string key_escape = escape_string(std::move(key_opt).value());
            result.emplace(std::move(key_escape), std::move(val));

            if (*_cur == ',') {
                ++_cur;
            }
            else {
                break;
            }
        }

        if (skip_whitespace() && *_cur == '}') {
            ++_cur;
        }
        else {
            return invalid_value();
        }

        return result;
    }

    std::optional<std::string> parser::parse_stdstring()
    {
        if (*_cur == '"') {
            ++_cur;
        }
        else {
            return std::nullopt;
        }

        const auto first = _cur;
        auto       last = _cur;
        bool       is_string_end = false;
        while (!is_string_end && _cur != _end) {
            switch (*_cur) {
            case '\t':
            case '\r':
            case '\n':
                return std::nullopt;
            case '\\': {
                if (++_cur == _end) {
                    return std::nullopt;
                }
                switch (*_cur) {
                case '"':
                case '\\':
                case '/':
                case 'b':
                case 'f':
                case 'n':
                case 'r':
                case 't':
                case 'u':
                    ++_cur;
                    break;
                default:
                    // Illegal backslash escape
                    return std::nullopt;
                }
                break;
            }
            case '"': {
                last = _cur;
                ++_cur;
                is_string_end = true;
                break;
            }
            default:
                ++_cur;
                break;
            }
        }
        if (_cur == _end) {
            return std::nullopt;
        }

        return std::string(first, last);
    }

    bool parser::skip_whitespace() noexcept
    {
        while (_cur != _end) {
            switch (*_cur) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                ++_cur;
                break;
            default:
                return true;
            }
        }
        return false;
    }

    bool parser::skip_digit()
    {
        // At least one digit
        if (_cur != _end && std::isdigit(*_cur)) {
            ++_cur;
        }
        else {
            return false;
        }

        while (_cur != _end && std::isdigit(*_cur)) {
            ++_cur;
        }

        if (_cur != _end) {
            return true;
        }
        else {
            return false;
        }
    }

    std::optional<value> parse(const std::string& content)
    {
        return parser::parse(content);
    }

#ifdef JSON5

    // *************************
    // *      json5 parser     *
    // *************************

    class parser5
    {
    private:
        using u8char = uint64_t;

        /* exceptions */
    public:
        class exception : public json::exception
        {
        public:
            exception(const std::string& type, const std::string& msg,
                      const std::string& detail)
                : _type(type), _msg(msg), _detail(detail)
            {
            }
            exception(const exception&) = default;
            exception& operator=(const exception&) = default;
            exception(exception&&) = default;
            exception& operator=(exception&&) = default;

            virtual ~exception() noexcept = default;

            virtual const char* what() const noexcept override
            {
                std::stringstream ss;
                ss << "JSON5: [" << _type << "] " << _msg << '\n';
                ss << _detail << std::endl;
                return ss.str().c_str();
            }

        protected:
            std::string _msg, _type, _detail;
        };

        class InvalidChar : public exception
        {
        public:
            InvalidChar(u8char ch = 0, const std::string& detail = "")
                : exception("Invalid Char",
                            "Unexpected token \'" + StringFromCharCode(ch) + "\'",
                            detail)
            {
            }
        };

        class InvalidIdentifier : public exception
        {
        public:
            InvalidIdentifier(const std::string& msg = "",
                              const std::string& detail = "")
                : exception("Invalid Identifier", msg, detail)
            {
            }
        };

        class InvalidEOF : public exception
        {
        public:
            InvalidEOF(const std::string& msg = "", const std::string& detail = "")
                : exception("Invalid EOF", msg, detail)
            {
            }
        };

    private:
        std::string exceptionDetailInfo()
        {
            size_t            start_point = _col;
            auto              len = _print_len;
            auto              current_line = _line_begin_cur;
            std::stringstream ss;
            ss << "at line " << _line << ", column " << _col + 1 << '\n';

            while (read() != '\n' && _cur != _end && (_col - start_point) < 5)
                ;
            ss << std::string(current_line, _cur) << '\n';
            ss << std::setw(len) << '^' << '\n';
            return ss.str();
        }

        /* helper class and type */
    private:
        class unicode
        {
        public:
            static const std::wregex space_separator;
            static const std::wregex id_start;
            static const std::wregex id_continue;

            static bool isSpaceSeparator(u8char ch);
            static bool isIdStartChar(u8char ch);
            static bool isIdContinueChar(u8char ch);
            static bool isDigit(u8char ch);
            static bool isHexDigit(u8char ch);
        };

        enum class LexState
        {
            default_ = 0,
            comment,
            multiLineComment,
            multiLineCommentAsterisk,
            singleLineComment,
            value,
            identifierNameStartEscape,
            identifierName,
            identifierNameEscape,
            sign,
            zero,
            decimalInteger,
            decimalPointLeading,
            decimalPoint,
            decimalFraction,
            decimalExponent,
            decimalExponentSign,
            decimalExponentInteger,
            hexadecimal,
            hexadecimalInteger,
            string,
            start,
            beforePropertyName,
            afterPropertyName,
            beforePropertyValue,
            afterPropertyValue,
            beforeArrayValue,
            afterArrayValue,
            end
        };

        enum class ParseState
        {
            start = 21,
            beforePropertyName,
            afterPropertyName,
            beforePropertyValue,
            afterPropertyValue,
            beforeArrayValue,
            afterArrayValue,
            end
        };

        enum class TokenType
        {
            punctuator = 0,
            identifier,
            null,
            boolean,
            numeric,
            string,
            eof,
        };

        struct Token
        {
            TokenType type;
            value     _value;
            size_t    col, line;
        };

        /* constrators and callers */
    public:
        ~parser5() noexcept = default;
        static std::optional<value> parse(const std::string& content);

    private:
        parser5(const std::string::const_iterator& cbegin,
                const std::string::const_iterator& cend) noexcept
            : _cur(cbegin), _end(cend), _line_begin_cur(cbegin)
        {
        }
        std::optional<value> parse();

    private:
        /* utf-8 reader */
        static u8char      peek(std::string::const_iterator& begin,
                                const std::string::const_iterator& end,
                                size_t* len = nullptr);
        static u8char      peek(const std::string& str);
        parser5::u8char    read();
        static std::string StringFromCharCode(u8char code);
        /* escape and format */
        void                  literal(const std::string& s);
        std::optional<u8char> escape();
        u8char                hexEscape();
        u8char                unicodeEscape();
        /* lex, parse, token */
        Token lex();
        Token newToken(TokenType type, value value);

        std::optional<Token> lex_default();
        std::optional<Token> lex_comment();
        std::optional<Token> lex_multiLineComment();
        std::optional<Token> lex_multiLineCommentAsterisk();
        std::optional<Token> lex_singleLineComment();
        std::optional<Token> lex_value();
        std::optional<Token> lex_identifierNameStartEscape();
        std::optional<Token> lex_identifierName();
        std::optional<Token> lex_identifierNameEscape();
        std::optional<Token> lex_sign();
        std::optional<Token> lex_zero();
        std::optional<Token> lex_decimalInteger();
        std::optional<Token> lex_decimalPointLeading();
        std::optional<Token> lex_decimalPoint();
        std::optional<Token> lex_decimalFraction();
        std::optional<Token> lex_decimalExponent();
        std::optional<Token> lex_decimalExponentSign();
        std::optional<Token> lex_decimalExponentInteger();
        std::optional<Token> lex_hexadecimal();
        std::optional<Token> lex_hexdecimalInteger();
        std::optional<Token> lex_string();
        std::optional<Token> lex_start();
        std::optional<Token> lex_beforePropertyName();
        std::optional<Token> lex_afterPropertyName();
        std::optional<Token> lex_beforePropertyValue();
        std::optional<Token> lex_afterPropertyValue();
        std::optional<Token> lex_beforeArrayValue();
        std::optional<Token> lex_afterArrayValue();
        std::optional<Token> lex_end();

        std::optional<Token> lexStates(LexState state);

        void parse_start();
        void parse_beforePropertyName();
        void parse_afterPropertyName();
        void parse_beforePropertyValue();
        void parse_beforeArrayValue();
        void parse_afterPropertyValue();
        void parse_afterArrayValue();
        void parse_end();

        void parseStates(ParseState state);

        /* stack operation */
        void push();
        void pop();

    private:
        std::string::const_iterator _cur, _end;
        std::string::const_iterator _line_begin_cur;
        size_t                      _line = 1, _col = 0, _print_len = 0;
        ParseState                  _parse_state = ParseState::start;
        std::stack<value*>         _stack;
        std::optional<Token>        token;
        std::string                 key;
        std::optional<value>        root;

        LexState    _lex_state = LexState::default_;
        std::string _buffer;
        bool        _double_quote = false;
        size_t      _sign = 1;
        u8char      _current_char;
    };

    /* parser5 impl */

    /* parser5::unicode */
    const std::wregex parser5::unicode::space_separator =
        std::wregex(LR"([\u1680\u2000-\u200A\u202F\u205F\u3000])");

    const std::wregex parser5::unicode::id_start = std::wregex(
        LR"([\xAA\xB5\xBA\xC0-\xD6\xD8-\xF6\xF8-\u02C1\u02C6-\u02D1\u02E0-)"
        LR"(\u02E4\u02EC\u02EE\u0370-\u0374\u0376\u0377\u037A-)"
        LR"(\u037D\u037F\u0386\u0388-\u038A\u038C\u038E-\u03A1\u03A3-\u03F5\u03F7-)"
        LR"(\u0481\u048A-\u052F\u0531-\u0556\u0559\u0561-\u0587\u05D0-\u05EA\u05F0-)"
        LR"(\u05F2\u0620-\u064A\u066E\u066F\u0671-)"
        LR"(\u06D3\u06D5\u06E5\u06E6\u06EE\u06EF\u06FA-\u06FC\u06FF\u0710\u0712-)"
        LR"(\u072F\u074D-\u07A5\u07B1\u07CA-\u07EA\u07F4\u07F5\u07FA\u0800-)"
        LR"(\u0815\u081A\u0824\u0828\u0840-\u0858\u0860-\u086A\u08A0-\u08B4\u08B6-)"
        LR"(\u08BD\u0904-\u0939\u093D\u0950\u0958-\u0961\u0971-\u0980\u0985-)"
        LR"(\u098C\u098F\u0990\u0993-\u09A8\u09AA-\u09B0\u09B2\u09B6-)"
        LR"(\u09B9\u09BD\u09CE\u09DC\u09DD\u09DF-\u09E1\u09F0\u09F1\u09FC\u0A05-)"
        LR"(\u0A0A\u0A0F\u0A10\u0A13-\u0A28\u0A2A-)"
        LR"(\u0A30\u0A32\u0A33\u0A35\u0A36\u0A38\u0A39\u0A59-\u0A5C\u0A5E\u0A72-)"
        LR"(\u0A74\u0A85-\u0A8D\u0A8F-\u0A91\u0A93-\u0AA8\u0AAA-)"
        LR"(\u0AB0\u0AB2\u0AB3\u0AB5-\u0AB9\u0ABD\u0AD0\u0AE0\u0AE1\u0AF9\u0B05-)"
        LR"(\u0B0C\u0B0F\u0B10\u0B13-\u0B28\u0B2A-\u0B30\u0B32\u0B33\u0B35-)"
        LR"(\u0B39\u0B3D\u0B5C\u0B5D\u0B5F-\u0B61\u0B71\u0B83\u0B85-\u0B8A\u0B8E-)"
        LR"(\u0B90\u0B92-\u0B95\u0B99\u0B9A\u0B9C\u0B9E\u0B9F\u0BA3\u0BA4\u0BA8-)"
        LR"(\u0BAA\u0BAE-\u0BB9\u0BD0\u0C05-\u0C0C\u0C0E-\u0C10\u0C12-\u0C28\u0C2A-)"
        LR"(\u0C39\u0C3D\u0C58-\u0C5A\u0C60\u0C61\u0C80\u0C85-\u0C8C\u0C8E-)"
        LR"(\u0C90\u0C92-\u0CA8\u0CAA-\u0CB3\u0CB5-)"
        LR"(\u0CB9\u0CBD\u0CDE\u0CE0\u0CE1\u0CF1\u0CF2\u0D05-\u0D0C\u0D0E-)"
        LR"(\u0D10\u0D12-\u0D3A\u0D3D\u0D4E\u0D54-\u0D56\u0D5F-\u0D61\u0D7A-)"
        LR"(\u0D7F\u0D85-\u0D96\u0D9A-\u0DB1\u0DB3-\u0DBB\u0DBD\u0DC0-\u0DC6\u0E01-)"
        LR"(\u0E30\u0E32\u0E33\u0E40-)"
        LR"(\u0E46\u0E81\u0E82\u0E84\u0E87\u0E88\u0E8A\u0E8D\u0E94-\u0E97\u0E99-)"
        LR"(\u0E9F\u0EA1-\u0EA3\u0EA5\u0EA7\u0EAA\u0EAB\u0EAD-)"
        LR"(\u0EB0\u0EB2\u0EB3\u0EBD\u0EC0-\u0EC4\u0EC6\u0EDC-\u0EDF\u0F00\u0F40-)"
        LR"(\u0F47\u0F49-\u0F6C\u0F88-\u0F8C\u1000-\u102A\u103F\u1050-\u1055\u105A-)"
        LR"(\u105D\u1061\u1065\u1066\u106E-\u1070\u1075-\u1081\u108E\u10A0-)"
        LR"(\u10C5\u10C7\u10CD\u10D0-\u10FA\u10FC-\u1248\u124A-\u124D\u1250-)"
        LR"(\u1256\u1258\u125A-\u125D\u1260-\u1288\u128A-\u128D\u1290-\u12B0\u12B2-)"
        LR"(\u12B5\u12B8-\u12BE\u12C0\u12C2-\u12C5\u12C8-\u12D6\u12D8-\u1310\u1312-)"
        LR"(\u1315\u1318-\u135A\u1380-\u138F\u13A0-\u13F5\u13F8-\u13FD\u1401-)"
        LR"(\u166C\u166F-\u167F\u1681-\u169A\u16A0-\u16EA\u16EE-\u16F8\u1700-)"
        LR"(\u170C\u170E-\u1711\u1720-\u1731\u1740-\u1751\u1760-\u176C\u176E-)"
        LR"(\u1770\u1780-\u17B3\u17D7\u17DC\u1820-\u1877\u1880-\u1884\u1887-)"
        LR"(\u18A8\u18AA\u18B0-\u18F5\u1900-\u191E\u1950-\u196D\u1970-\u1974\u1980-)"
        LR"(\u19AB\u19B0-\u19C9\u1A00-\u1A16\u1A20-\u1A54\u1AA7\u1B05-\u1B33\u1B45-)"
        LR"(\u1B4B\u1B83-\u1BA0\u1BAE\u1BAF\u1BBA-\u1BE5\u1C00-\u1C23\u1C4D-)"
        LR"(\u1C4F\u1C5A-\u1C7D\u1C80-\u1C88\u1CE9-\u1CEC\u1CEE-)"
        LR"(\u1CF1\u1CF5\u1CF6\u1D00-\u1DBF\u1E00-\u1F15\u1F18-\u1F1D\u1F20-)"
        LR"(\u1F45\u1F48-\u1F4D\u1F50-\u1F57\u1F59\u1F5B\u1F5D\u1F5F-\u1F7D\u1F80-)"
        LR"(\u1FB4\u1FB6-\u1FBC\u1FBE\u1FC2-\u1FC4\u1FC6-\u1FCC\u1FD0-\u1FD3\u1FD6-)"
        LR"(\u1FDB\u1FE0-\u1FEC\u1FF2-\u1FF4\u1FF6-\u1FFC\u2071\u207F\u2090-)"
        LR"(\u209C\u2102\u2107\u210A-\u2113\u2115\u2119-)"
        LR"(\u211D\u2124\u2126\u2128\u212A-\u212D\u212F-\u2139\u213C-\u213F\u2145-)"
        LR"(\u2149\u214E\u2160-\u2188\u2C00-\u2C2E\u2C30-\u2C5E\u2C60-\u2CE4\u2CEB-)"
        LR"(\u2CEE\u2CF2\u2CF3\u2D00-\u2D25\u2D27\u2D2D\u2D30-\u2D67\u2D6F\u2D80-)"
        LR"(\u2D96\u2DA0-\u2DA6\u2DA8-\u2DAE\u2DB0-\u2DB6\u2DB8-\u2DBE\u2DC0-)"
        LR"(\u2DC6\u2DC8-\u2DCE\u2DD0-\u2DD6\u2DD8-\u2DDE\u2E2F\u3005-\u3007\u3021-)"
        LR"(\u3029\u3031-\u3035\u3038-\u303C\u3041-\u3096\u309D-\u309F\u30A1-)"
        LR"(\u30FA\u30FC-\u30FF\u3105-\u312E\u3131-\u318E\u31A0-\u31BA\u31F0-)"
        LR"(\u31FF\u3400-\u4DB5\u4E00-\u9FEA\uA000-\uA48C\uA4D0-\uA4FD\uA500-)"
        LR"(\uA60C\uA610-\uA61F\uA62A\uA62B\uA640-\uA66E\uA67F-\uA69D\uA6A0-)"
        LR"(\uA6EF\uA717-\uA71F\uA722-\uA788\uA78B-\uA7AE\uA7B0-\uA7B7\uA7F7-)"
        LR"(\uA801\uA803-\uA805\uA807-\uA80A\uA80C-\uA822\uA840-\uA873\uA882-)"
        LR"(\uA8B3\uA8F2-\uA8F7\uA8FB\uA8FD\uA90A-\uA925\uA930-\uA946\uA960-)"
        LR"(\uA97C\uA984-\uA9B2\uA9CF\uA9E0-\uA9E4\uA9E6-\uA9EF\uA9FA-\uA9FE\uAA00-)"
        LR"(\uAA28\uAA40-\uAA42\uAA44-\uAA4B\uAA60-\uAA76\uAA7A\uAA7E-)"
        LR"(\uAAAF\uAAB1\uAAB5\uAAB6\uAAB9-\uAABD\uAAC0\uAAC2\uAADB-\uAADD\uAAE0-)"
        LR"(\uAAEA\uAAF2-\uAAF4\uAB01-\uAB06\uAB09-\uAB0E\uAB11-\uAB16\uAB20-)"
        LR"(\uAB26\uAB28-\uAB2E\uAB30-\uAB5A\uAB5C-\uAB65\uAB70-\uABE2\uAC00-)"
        LR"(\uD7A3\uD7B0-\uD7C6\uD7CB-\uD7FB\uF900-\uFA6D\uFA70-\uFAD9\uFB00-)"
        LR"(\uFB06\uFB13-\uFB17\uFB1D\uFB1F-\uFB28\uFB2A-\uFB36\uFB38-)"
        LR"(\uFB3C\uFB3E\uFB40\uFB41\uFB43\uFB44\uFB46-\uFBB1\uFBD3-\uFD3D\uFD50-)"
        LR"(\uFD8F\uFD92-\uFDC7\uFDF0-\uFDFB\uFE70-\uFE74\uFE76-\uFEFC\uFF21-)"
        LR"(\uFF3A\uFF41-\uFF5A\uFF66-\uFFBE\uFFC2-\uFFC7\uFFCA-\uFFCF\uFFD2-)"
        LR"(\uFFD7\uFFDA-\uFFDC]|\uD800[\uDC00-\uDC0B\uDC0D-\uDC26\uDC28-)"
        LR"(\uDC3A\uDC3C\uDC3D\uDC3F-\uDC4D\uDC50-\uDC5D\uDC80-\uDCFA\uDD40-)"
        LR"(\uDD74\uDE80-\uDE9C\uDEA0-\uDED0\uDF00-\uDF1F\uDF2D-\uDF4A\uDF50-)"
        LR"(\uDF75\uDF80-\uDF9D\uDFA0-\uDFC3\uDFC8-\uDFCF\uDFD1-\uDFD5]|\uD801[\uDC00-)"
        LR"(\uDC9D\uDCB0-\uDCD3\uDCD8-\uDCFB\uDD00-\uDD27\uDD30-\uDD63\uDE00-)"
        LR"(\uDF36\uDF40-\uDF55\uDF60-\uDF67]|\uD802[\uDC00-\uDC05\uDC08\uDC0A-)"
        LR"(\uDC35\uDC37\uDC38\uDC3C\uDC3F-\uDC55\uDC60-\uDC76\uDC80-\uDC9E\uDCE0-)"
        LR"(\uDCF2\uDCF4\uDCF5\uDD00-\uDD15\uDD20-\uDD39\uDD80-)"
        LR"(\uDDB7\uDDBE\uDDBF\uDE00\uDE10-\uDE13\uDE15-\uDE17\uDE19-\uDE33\uDE60-)"
        LR"(\uDE7C\uDE80-\uDE9C\uDEC0-\uDEC7\uDEC9-\uDEE4\uDF00-\uDF35\uDF40-)"
        LR"(\uDF55\uDF60-\uDF72\uDF80-\uDF91]|\uD803[\uDC00-\uDC48\uDC80-\uDCB2\uDCC0-)"
        LR"(\uDCF2]|\uD804[\uDC03-\uDC37\uDC83-\uDCAF\uDCD0-\uDCE8\uDD03-\uDD26\uDD50-)"
        LR"(\uDD72\uDD76\uDD83-\uDDB2\uDDC1-\uDDC4\uDDDA\uDDDC\uDE00-\uDE11\uDE13-)"
        LR"(\uDE2B\uDE80-\uDE86\uDE88\uDE8A-\uDE8D\uDE8F-\uDE9D\uDE9F-\uDEA8\uDEB0-)"
        LR"(\uDEDE\uDF05-\uDF0C\uDF0F\uDF10\uDF13-\uDF28\uDF2A-)"
        LR"(\uDF30\uDF32\uDF33\uDF35-\uDF39\uDF3D\uDF50\uDF5D-\uDF61]|\uD805[\uDC00-)"
        LR"(\uDC34\uDC47-\uDC4A\uDC80-\uDCAF\uDCC4\uDCC5\uDCC7\uDD80-\uDDAE\uDDD8-)"
        LR"(\uDDDB\uDE00-\uDE2F\uDE44\uDE80-\uDEAA\uDF00-\uDF19]|\uD806[\uDCA0-)"
        LR"(\uDCDF\uDCFF\uDE00\uDE0B-\uDE32\uDE3A\uDE50\uDE5C-\uDE83\uDE86-)"
        LR"(\uDE89\uDEC0-\uDEF8]|\uD807[\uDC00-\uDC08\uDC0A-\uDC2E\uDC40\uDC72-)"
        LR"(\uDC8F\uDD00-\uDD06\uDD08\uDD09\uDD0B-\uDD30\uDD46]|\uD808[\uDC00-\uDF99]|)"
        LR"(\uD809[\uDC00-\uDC6E\uDC80-\uDD43]|[\uD80C\uD81C-\uD820\uD840-)"
        LR"(\uD868\uD86A-\uD86C\uD86F-\uD872\uD874-\uD879][\uDC00-\uDFFF]|\uD80D[)"
        LR"(\uDC00-\uDC2E]|\uD811[\uDC00-\uDE46]|\uD81A[\uDC00-\uDE38\uDE40-)"
        LR"(\uDE5E\uDED0-\uDEED\uDF00-\uDF2F\uDF40-\uDF43\uDF63-\uDF77\uDF7D-\uDF8F]|)"
        LR"(\uD81B[\uDF00-\uDF44\uDF50\uDF93-\uDF9F\uDFE0\uDFE1]|\uD821[\uDC00-\uDFEC])"
        LR"(|\uD822[\uDC00-\uDEF2]|\uD82C[\uDC00-\uDD1E\uDD70-\uDEFB]|\uD82F[\uDC00-)"
        LR"(\uDC6A\uDC70-\uDC7C\uDC80-\uDC88\uDC90-\uDC99]|\uD835[\uDC00-\uDC54\uDC56-)"
        LR"(\uDC9C\uDC9E\uDC9F\uDCA2\uDCA5\uDCA6\uDCA9-\uDCAC\uDCAE-)"
        LR"(\uDCB9\uDCBB\uDCBD-\uDCC3\uDCC5-\uDD05\uDD07-\uDD0A\uDD0D-\uDD14\uDD16-)"
        LR"(\uDD1C\uDD1E-\uDD39\uDD3B-\uDD3E\uDD40-\uDD44\uDD46\uDD4A-\uDD50\uDD52-)"
        LR"(\uDEA5\uDEA8-\uDEC0\uDEC2-\uDEDA\uDEDC-\uDEFA\uDEFC-\uDF14\uDF16-)"
        LR"(\uDF34\uDF36-\uDF4E\uDF50-\uDF6E\uDF70-\uDF88\uDF8A-\uDFA8\uDFAA-)"
        LR"(\uDFC2\uDFC4-\uDFCB]|\uD83A[\uDC00-\uDCC4\uDD00-\uDD43]|\uD83B[\uDE00-)"
        LR"(\uDE03\uDE05-\uDE1F\uDE21\uDE22\uDE24\uDE27\uDE29-\uDE32\uDE34-)"
        LR"(\uDE37\uDE39\uDE3B\uDE42\uDE47\uDE49\uDE4B\uDE4D-)"
        LR"(\uDE4F\uDE51\uDE52\uDE54\uDE57\uDE59\uDE5B\uDE5D\uDE5F\uDE61\uDE62\uDE64)"
        LR"(\uDE67-\uDE6A\uDE6C-\uDE72\uDE74-\uDE77\uDE79-\uDE7C\uDE7E\uDE80-)"
        LR"(\uDE89\uDE8B-\uDE9B\uDEA1-\uDEA3\uDEA5-\uDEA9\uDEAB-\uDEBB]|\uD869[\uDC00-)"
        LR"(\uDED6\uDF00-\uDFFF]|\uD86D[\uDC00-\uDF34\uDF40-\uDFFF]|\uD86E[\uDC00-)"
        LR"(\uDC1D\uDC20-\uDFFF]|\uD873[\uDC00-\uDEA1\uDEB0-\uDFFF]|\uD87A[\uDC00-)"
        LR"(\uDFE0]|\uD87E[\uDC00-\uDE1D])");
    const std::wregex parser5::unicode::id_continue = std::wregex(
        LR"([\xAA\xB5\xBA\xC0-\xD6\xD8-\xF6\xF8-\u02C1\u02C6-\u02D1\u02E0-)"
        LR"(\u02E4\u02EC\u02EE\u0300-\u0374\u0376\u0377\u037A-)"
        LR"(\u037D\u037F\u0386\u0388-\u038A\u038C\u038E-\u03A1\u03A3-\u03F5\u03F7-)"
        LR"(\u0481\u0483-\u0487\u048A-\u052F\u0531-\u0556\u0559\u0561-\u0587\u0591-)"
        LR"(\u05BD\u05BF\u05C1\u05C2\u05C4\u05C5\u05C7\u05D0-\u05EA\u05F0-)"
        LR"(\u05F2\u0610-\u061A\u0620-\u0669\u066E-\u06D3\u06D5-\u06DC\u06DF-)"
        LR"(\u06E8\u06EA-\u06FC\u06FF\u0710-\u074A\u074D-\u07B1\u07C0-)"
        LR"(\u07F5\u07FA\u0800-\u082D\u0840-\u085B\u0860-\u086A\u08A0-\u08B4\u08B6-)"
        LR"(\u08BD\u08D4-\u08E1\u08E3-\u0963\u0966-\u096F\u0971-\u0983\u0985-)"
        LR"(\u098C\u098F\u0990\u0993-\u09A8\u09AA-\u09B0\u09B2\u09B6-\u09B9\u09BC-)"
        LR"(\u09C4\u09C7\u09C8\u09CB-\u09CE\u09D7\u09DC\u09DD\u09DF-\u09E3\u09E6-)"
        LR"(\u09F1\u09FC\u0A01-\u0A03\u0A05-\u0A0A\u0A0F\u0A10\u0A13-\u0A28\u0A2A-)"
        LR"(\u0A30\u0A32\u0A33\u0A35\u0A36\u0A38\u0A39\u0A3C\u0A3E-)"
        LR"(\u0A42\u0A47\u0A48\u0A4B-\u0A4D\u0A51\u0A59-\u0A5C\u0A5E\u0A66-)"
        LR"(\u0A75\u0A81-\u0A83\u0A85-\u0A8D\u0A8F-\u0A91\u0A93-\u0AA8\u0AAA-)"
        LR"(\u0AB0\u0AB2\u0AB3\u0AB5-\u0AB9\u0ABC-\u0AC5\u0AC7-\u0AC9\u0ACB-)"
        LR"(\u0ACD\u0AD0\u0AE0-\u0AE3\u0AE6-\u0AEF\u0AF9-\u0AFF\u0B01-\u0B03\u0B05-)"
        LR"(\u0B0C\u0B0F\u0B10\u0B13-\u0B28\u0B2A-\u0B30\u0B32\u0B33\u0B35-)"
        LR"(\u0B39\u0B3C-\u0B44\u0B47\u0B48\u0B4B-)"
        LR"(\u0B4D\u0B56\u0B57\u0B5C\u0B5D\u0B5F-\u0B63\u0B66-)"
        LR"(\u0B6F\u0B71\u0B82\u0B83\u0B85-\u0B8A\u0B8E-\u0B90\u0B92-)"
        LR"(\u0B95\u0B99\u0B9A\u0B9C\u0B9E\u0B9F\u0BA3\u0BA4\u0BA8-\u0BAA\u0BAE-)"
        LR"(\u0BB9\u0BBE-\u0BC2\u0BC6-\u0BC8\u0BCA-\u0BCD\u0BD0\u0BD7\u0BE6-)"
        LR"(\u0BEF\u0C00-\u0C03\u0C05-\u0C0C\u0C0E-\u0C10\u0C12-\u0C28\u0C2A-)"
        LR"(\u0C39\u0C3D-\u0C44\u0C46-\u0C48\u0C4A-\u0C4D\u0C55\u0C56\u0C58-)"
        LR"(\u0C5A\u0C60-\u0C63\u0C66-\u0C6F\u0C80-\u0C83\u0C85-\u0C8C\u0C8E-)"
        LR"(\u0C90\u0C92-\u0CA8\u0CAA-\u0CB3\u0CB5-\u0CB9\u0CBC-\u0CC4\u0CC6-)"
        LR"(\u0CC8\u0CCA-\u0CCD\u0CD5\u0CD6\u0CDE\u0CE0-\u0CE3\u0CE6-)"
        LR"(\u0CEF\u0CF1\u0CF2\u0D00-\u0D03\u0D05-\u0D0C\u0D0E-\u0D10\u0D12-)"
        LR"(\u0D44\u0D46-\u0D48\u0D4A-\u0D4E\u0D54-\u0D57\u0D5F-\u0D63\u0D66-)"
        LR"(\u0D6F\u0D7A-\u0D7F\u0D82\u0D83\u0D85-\u0D96\u0D9A-\u0DB1\u0DB3-)"
        LR"(\u0DBB\u0DBD\u0DC0-\u0DC6\u0DCA\u0DCF-\u0DD4\u0DD6\u0DD8-\u0DDF\u0DE6-)"
        LR"(\u0DEF\u0DF2\u0DF3\u0E01-\u0E3A\u0E40-\u0E4E\u0E50-)"
        LR"(\u0E59\u0E81\u0E82\u0E84\u0E87\u0E88\u0E8A\u0E8D\u0E94-\u0E97\u0E99-)"
        LR"(\u0E9F\u0EA1-\u0EA3\u0EA5\u0EA7\u0EAA\u0EAB\u0EAD-\u0EB9\u0EBB-)"
        LR"(\u0EBD\u0EC0-\u0EC4\u0EC6\u0EC8-\u0ECD\u0ED0-\u0ED9\u0EDC-)"
        LR"(\u0EDF\u0F00\u0F18\u0F19\u0F20-\u0F29\u0F35\u0F37\u0F39\u0F3E-)"
        LR"(\u0F47\u0F49-\u0F6C\u0F71-\u0F84\u0F86-\u0F97\u0F99-\u0FBC\u0FC6\u1000-)"
        LR"(\u1049\u1050-\u109D\u10A0-\u10C5\u10C7\u10CD\u10D0-\u10FA\u10FC-)"
        LR"(\u1248\u124A-\u124D\u1250-\u1256\u1258\u125A-\u125D\u1260-\u1288\u128A-)"
        LR"(\u128D\u1290-\u12B0\u12B2-\u12B5\u12B8-\u12BE\u12C0\u12C2-\u12C5\u12C8-)"
        LR"(\u12D6\u12D8-\u1310\u1312-\u1315\u1318-\u135A\u135D-\u135F\u1380-)"
        LR"(\u138F\u13A0-\u13F5\u13F8-\u13FD\u1401-\u166C\u166F-\u167F\u1681-)"
        LR"(\u169A\u16A0-\u16EA\u16EE-\u16F8\u1700-\u170C\u170E-\u1714\u1720-)"
        LR"(\u1734\u1740-\u1753\u1760-\u176C\u176E-\u1770\u1772\u1773\u1780-)"
        LR"(\u17D3\u17D7\u17DC\u17DD\u17E0-\u17E9\u180B-\u180D\u1810-\u1819\u1820-)"
        LR"(\u1877\u1880-\u18AA\u18B0-\u18F5\u1900-\u191E\u1920-\u192B\u1930-)"
        LR"(\u193B\u1946-\u196D\u1970-\u1974\u1980-\u19AB\u19B0-\u19C9\u19D0-)"
        LR"(\u19D9\u1A00-\u1A1B\u1A20-\u1A5E\u1A60-\u1A7C\u1A7F-\u1A89\u1A90-)"
        LR"(\u1A99\u1AA7\u1AB0-\u1ABD\u1B00-\u1B4B\u1B50-\u1B59\u1B6B-\u1B73\u1B80-)"
        LR"(\u1BF3\u1C00-\u1C37\u1C40-\u1C49\u1C4D-\u1C7D\u1C80-\u1C88\u1CD0-)"
        LR"(\u1CD2\u1CD4-\u1CF9\u1D00-\u1DF9\u1DFB-\u1F15\u1F18-\u1F1D\u1F20-)"
        LR"(\u1F45\u1F48-\u1F4D\u1F50-\u1F57\u1F59\u1F5B\u1F5D\u1F5F-\u1F7D\u1F80-)"
        LR"(\u1FB4\u1FB6-\u1FBC\u1FBE\u1FC2-\u1FC4\u1FC6-\u1FCC\u1FD0-\u1FD3\u1FD6-)"
        LR"(\u1FDB\u1FE0-\u1FEC\u1FF2-\u1FF4\u1FF6-)"
        LR"(\u1FFC\u203F\u2040\u2054\u2071\u207F\u2090-\u209C\u20D0-)"
        LR"(\u20DC\u20E1\u20E5-\u20F0\u2102\u2107\u210A-\u2113\u2115\u2119-)"
        LR"(\u211D\u2124\u2126\u2128\u212A-\u212D\u212F-\u2139\u213C-\u213F\u2145-)"
        LR"(\u2149\u214E\u2160-\u2188\u2C00-\u2C2E\u2C30-\u2C5E\u2C60-\u2CE4\u2CEB-)"
        LR"(\u2CF3\u2D00-\u2D25\u2D27\u2D2D\u2D30-\u2D67\u2D6F\u2D7F-\u2D96\u2DA0-)"
        LR"(\u2DA6\u2DA8-\u2DAE\u2DB0-\u2DB6\u2DB8-\u2DBE\u2DC0-\u2DC6\u2DC8-)"
        LR"(\u2DCE\u2DD0-\u2DD6\u2DD8-\u2DDE\u2DE0-\u2DFF\u2E2F\u3005-\u3007\u3021-)"
        LR"(\u302F\u3031-\u3035\u3038-\u303C\u3041-\u3096\u3099\u309A\u309D-)"
        LR"(\u309F\u30A1-\u30FA\u30FC-\u30FF\u3105-\u312E\u3131-\u318E\u31A0-)"
        LR"(\u31BA\u31F0-\u31FF\u3400-\u4DB5\u4E00-\u9FEA\uA000-\uA48C\uA4D0-)"
        LR"(\uA4FD\uA500-\uA60C\uA610-\uA62B\uA640-\uA66F\uA674-\uA67D\uA67F-)"
        LR"(\uA6F1\uA717-\uA71F\uA722-\uA788\uA78B-\uA7AE\uA7B0-\uA7B7\uA7F7-)"
        LR"(\uA827\uA840-\uA873\uA880-\uA8C5\uA8D0-\uA8D9\uA8E0-)"
        LR"(\uA8F7\uA8FB\uA8FD\uA900-\uA92D\uA930-\uA953\uA960-\uA97C\uA980-)"
        LR"(\uA9C0\uA9CF-\uA9D9\uA9E0-\uA9FE\uAA00-\uAA36\uAA40-\uAA4D\uAA50-)"
        LR"(\uAA59\uAA60-\uAA76\uAA7A-\uAAC2\uAADB-\uAADD\uAAE0-\uAAEF\uAAF2-)"
        LR"(\uAAF6\uAB01-\uAB06\uAB09-\uAB0E\uAB11-\uAB16\uAB20-\uAB26\uAB28-)"
        LR"(\uAB2E\uAB30-\uAB5A\uAB5C-\uAB65\uAB70-\uABEA\uABEC\uABED\uABF0-)"
        LR"(\uABF9\uAC00-\uD7A3\uD7B0-\uD7C6\uD7CB-\uD7FB\uF900-\uFA6D\uFA70-)"
        LR"(\uFAD9\uFB00-\uFB06\uFB13-\uFB17\uFB1D-\uFB28\uFB2A-\uFB36\uFB38-)"
        LR"(\uFB3C\uFB3E\uFB40\uFB41\uFB43\uFB44\uFB46-\uFBB1\uFBD3-\uFD3D\uFD50-)"
        LR"(\uFD8F\uFD92-\uFDC7\uFDF0-\uFDFB\uFE00-\uFE0F\uFE20-)"
        LR"(\uFE2F\uFE33\uFE34\uFE4D-\uFE4F\uFE70-\uFE74\uFE76-\uFEFC\uFF10-)"
        LR"(\uFF19\uFF21-\uFF3A\uFF3F\uFF41-\uFF5A\uFF66-\uFFBE\uFFC2-\uFFC7\uFFCA-)"
        LR"(\uFFCF\uFFD2-\uFFD7\uFFDA-\uFFDC]|\uD800[\uDC00-\uDC0B\uDC0D-\uDC26\uDC28-)"
        LR"(\uDC3A\uDC3C\uDC3D\uDC3F-\uDC4D\uDC50-\uDC5D\uDC80-\uDCFA\uDD40-)"
        LR"(\uDD74\uDDFD\uDE80-\uDE9C\uDEA0-\uDED0\uDEE0\uDF00-\uDF1F\uDF2D-)"
        LR"(\uDF4A\uDF50-\uDF7A\uDF80-\uDF9D\uDFA0-\uDFC3\uDFC8-\uDFCF\uDFD1-\uDFD5]|)"
        LR"(\uD801[\uDC00-\uDC9D\uDCA0-\uDCA9\uDCB0-\uDCD3\uDCD8-\uDCFB\uDD00-)"
        LR"(\uDD27\uDD30-\uDD63\uDE00-\uDF36\uDF40-\uDF55\uDF60-\uDF67]|\uD802[\uDC00-)"
        LR"(\uDC05\uDC08\uDC0A-\uDC35\uDC37\uDC38\uDC3C\uDC3F-\uDC55\uDC60-)"
        LR"(\uDC76\uDC80-\uDC9E\uDCE0-\uDCF2\uDCF4\uDCF5\uDD00-\uDD15\uDD20-)"
        LR"(\uDD39\uDD80-\uDDB7\uDDBE\uDDBF\uDE00-\uDE03\uDE05\uDE06\uDE0C-)"
        LR"(\uDE13\uDE15-\uDE17\uDE19-\uDE33\uDE38-\uDE3A\uDE3F\uDE60-\uDE7C\uDE80-)"
        LR"(\uDE9C\uDEC0-\uDEC7\uDEC9-\uDEE6\uDF00-\uDF35\uDF40-\uDF55\uDF60-)"
        LR"(\uDF72\uDF80-\uDF91]|\uD803[\uDC00-\uDC48\uDC80-\uDCB2\uDCC0-\uDCF2]|)"
        LR"(\uD804[\uDC00-\uDC46\uDC66-\uDC6F\uDC7F-\uDCBA\uDCD0-\uDCE8\uDCF0-)"
        LR"(\uDCF9\uDD00-\uDD34\uDD36-\uDD3F\uDD50-\uDD73\uDD76\uDD80-\uDDC4\uDDCA-)"
        LR"(\uDDCC\uDDD0-\uDDDA\uDDDC\uDE00-\uDE11\uDE13-\uDE37\uDE3E\uDE80-)"
        LR"(\uDE86\uDE88\uDE8A-\uDE8D\uDE8F-\uDE9D\uDE9F-\uDEA8\uDEB0-\uDEEA\uDEF0-)"
        LR"(\uDEF9\uDF00-\uDF03\uDF05-\uDF0C\uDF0F\uDF10\uDF13-\uDF28\uDF2A-)"
        LR"(\uDF30\uDF32\uDF33\uDF35-\uDF39\uDF3C-\uDF44\uDF47\uDF48\uDF4B-)"
        LR"(\uDF4D\uDF50\uDF57\uDF5D-\uDF63\uDF66-\uDF6C\uDF70-\uDF74]|\uD805[\uDC00-)"
        LR"(\uDC4A\uDC50-\uDC59\uDC80-\uDCC5\uDCC7\uDCD0-\uDCD9\uDD80-\uDDB5\uDDB8-)"
        LR"(\uDDC0\uDDD8-\uDDDD\uDE00-\uDE40\uDE44\uDE50-\uDE59\uDE80-\uDEB7\uDEC0-)"
        LR"(\uDEC9\uDF00-\uDF19\uDF1D-\uDF2B\uDF30-\uDF39]|\uD806[\uDCA0-)"
        LR"(\uDCE9\uDCFF\uDE00-\uDE3E\uDE47\uDE50-\uDE83\uDE86-\uDE99\uDEC0-\uDEF8]|)"
        LR"(\uD807[\uDC00-\uDC08\uDC0A-\uDC36\uDC38-\uDC40\uDC50-\uDC59\uDC72-)"
        LR"(\uDC8F\uDC92-\uDCA7\uDCA9-\uDCB6\uDD00-\uDD06\uDD08\uDD09\uDD0B-)"
        LR"(\uDD36\uDD3A\uDD3C\uDD3D\uDD3F-\uDD47\uDD50-\uDD59]|\uD808[\uDC00-\uDF99]|)"
        LR"(\uD809[\uDC00-\uDC6E\uDC80-\uDD43]|[\uD80C\uD81C-\uD820\uD840-)"
        LR"(\uD868\uD86A-\uD86C\uD86F-\uD872\uD874-\uD879][\uDC00-\uDFFF]|\uD80D[)"
        LR"(\uDC00-\uDC2E]|\uD811[\uDC00-\uDE46]|\uD81A[\uDC00-\uDE38\uDE40-)"
        LR"(\uDE5E\uDE60-\uDE69\uDED0-\uDEED\uDEF0-\uDEF4\uDF00-\uDF36\uDF40-)"
        LR"(\uDF43\uDF50-\uDF59\uDF63-\uDF77\uDF7D-\uDF8F]|\uD81B[\uDF00-\uDF44\uDF50-)"
        LR"(\uDF7E\uDF8F-\uDF9F\uDFE0\uDFE1]|\uD821[\uDC00-\uDFEC]|\uD822[\uDC00-)"
        LR"(\uDEF2]|\uD82C[\uDC00-\uDD1E\uDD70-\uDEFB]|\uD82F[\uDC00-\uDC6A\uDC70-)"
        LR"(\uDC7C\uDC80-\uDC88\uDC90-\uDC99\uDC9D\uDC9E]|\uD834[\uDD65-\uDD69\uDD6D-)"
        LR"(\uDD72\uDD7B-\uDD82\uDD85-\uDD8B\uDDAA-\uDDAD\uDE42-\uDE44]|\uD835[\uDC00-)"
        LR"(\uDC54\uDC56-\uDC9C\uDC9E\uDC9F\uDCA2\uDCA5\uDCA6\uDCA9-\uDCAC\uDCAE-)"
        LR"(\uDCB9\uDCBB\uDCBD-\uDCC3\uDCC5-\uDD05\uDD07-\uDD0A\uDD0D-\uDD14\uDD16-)"
        LR"(\uDD1C\uDD1E-\uDD39\uDD3B-\uDD3E\uDD40-\uDD44\uDD46\uDD4A-\uDD50\uDD52-)"
        LR"(\uDEA5\uDEA8-\uDEC0\uDEC2-\uDEDA\uDEDC-\uDEFA\uDEFC-\uDF14\uDF16-)"
        LR"(\uDF34\uDF36-\uDF4E\uDF50-\uDF6E\uDF70-\uDF88\uDF8A-\uDFA8\uDFAA-)"
        LR"(\uDFC2\uDFC4-\uDFCB\uDFCE-\uDFFF]|\uD836[\uDE00-\uDE36\uDE3B-)"
        LR"(\uDE6C\uDE75\uDE84\uDE9B-\uDE9F\uDEA1-\uDEAF]|\uD838[\uDC00-\uDC06\uDC08-)"
        LR"(\uDC18\uDC1B-\uDC21\uDC23\uDC24\uDC26-\uDC2A]|\uD83A[\uDC00-\uDCC4\uDCD0-)"
        LR"(\uDCD6\uDD00-\uDD4A\uDD50-\uDD59]|\uD83B[\uDE00-\uDE03\uDE05-)"
        LR"(\uDE1F\uDE21\uDE22\uDE24\uDE27\uDE29-\uDE32\uDE34-)"
        LR"(\uDE37\uDE39\uDE3B\uDE42\uDE47\uDE49\uDE4B\uDE4D-)"
        LR"(\uDE4F\uDE51\uDE52\uDE54\uDE57\uDE59\uDE5B\uDE5D\uDE5F\uDE61\uDE62\uDE64)"
        LR"(\uDE67-\uDE6A\uDE6C-\uDE72\uDE74-\uDE77\uDE79-\uDE7C\uDE7E\uDE80-)"
        LR"(\uDE89\uDE8B-\uDE9B\uDEA1-\uDEA3\uDEA5-\uDEA9\uDEAB-\uDEBB]|\uD869[\uDC00-)"
        LR"(\uDED6\uDF00-\uDFFF]|\uD86D[\uDC00-\uDF34\uDF40-\uDFFF]|\uD86E[\uDC00-)"
        LR"(\uDC1D\uDC20-\uDFFF]|\uD873[\uDC00-\uDEA1\uDEB0-\uDFFF]|\uD87A[\uDC00-)"
        LR"(\uDFE0]|\uD87E[\uDC00-\uDE1D]|\uDB40[\uDD00-\uDDEF]|[\u200C\u200D])");

    bool parser5::unicode::isSpaceSeparator(u8char ch)
    {
#ifdef _MSC_VER
        std::wstring wstr = { (wchar_t)ch, 0 };
#else
        auto                       str = StringFromCharCode(ch);
        auto                       len = str.size() + 1;
        std::unique_ptr<wchar_t[]> p(new wchar_t[len]);
        std::setlocale(LC_CTYPE, ".UTF8");
        std::mbstowcs(p.get(), str.c_str(), len);
        std::wstring               wstr(p.get());
#endif
        return std::regex_search(wstr, unicode::space_separator);
    }

    bool parser5::unicode::isIdStartChar(u8char ch)
    {
#ifdef _MSC_VER
        std::wstring wstr = { (wchar_t)ch, 0 };
#else
        auto                       str = StringFromCharCode(ch);
        auto                       len = str.size() + 1;
        std::unique_ptr<wchar_t[]> p(new wchar_t[len]);
        std::setlocale(LC_CTYPE, ".UTF8");
        std::mbstowcs(p.get(), str.c_str(), len);
        std::wstring               wstr(p.get());
#endif
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '$') ||
            (ch == '_') || std::regex_search(wstr, unicode::id_start);
    }

    bool parser5::unicode::isIdContinueChar(u8char ch)
    {
#ifdef _MSC_VER
        std::wstring wstr = { (wchar_t)ch, 0 };
#else
        auto                       str = StringFromCharCode(ch);
        auto                       len = str.size() + 1;
        std::unique_ptr<wchar_t[]> p(new wchar_t[len]);
        std::setlocale(LC_CTYPE, ".UTF8");
        std::mbstowcs(p.get(), str.c_str(), len);
        std::wstring wstr(p.get());
#endif
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || (ch == '$') || (ch == '_') ||
            std::regex_search(wstr, unicode::id_continue);
    }

    bool parser5::unicode::isDigit(u8char ch)
    {
        auto str = StringFromCharCode(ch);
        return std::regex_search(str, std::regex(R"([0-9])"));
    }

    bool parser5::unicode::isHexDigit(u8char ch)
    {
        auto str = StringFromCharCode(ch);
        return std::regex_search(str, std::regex(R"([0-9A-Fa-f])"));
    }

    /* constrators and callers */
    std::optional<value> parse5(const std::string& content)
    {
        return parser5::parse(content);
    }

    std::optional<value> parser5::parse(const std::string& content)
    {
        return parser5(content.cbegin(), content.cend()).parse();
    }

    std::optional<value> parser5::parse()
    {
        do {
            token = lex();
            parseStates(_parse_state);
        } while (token->type != TokenType::eof);

        return root;
    }

    /* escape and format */
    void parser5::literal(const std::string& s)
    {
        for (const auto& ch : s) {
            auto p = read();
            if (p != ch) {
                throw InvalidChar(_current_char, exceptionDetailInfo());
            }
        }
    }

    std::optional<parser5::u8char> parser5::escape()
    {
        auto c = peek(_cur, _end);
        switch (c) {
        case 'b':
            read();
            return '\b';

        case 'f':
            read();
            return '\f';

        case 'n':
            read();
            return '\n';

        case 'r':
            read();
            return '\r';

        case 't':
            read();
            return '\t';

        case 'v':
            read();
            return '\v';

        case '0':
            read();
            if (unicode::isDigit(peek(_cur, _end))) {
                throw InvalidChar(_current_char, exceptionDetailInfo());
            }

            return '\0';

        case 'x':
            read();
            return hexEscape();

        case 'u':
            read();
            return unicodeEscape();

        case '\n':
        case 0x2028:
        case 0x2029:
            read();
            return 0;

        case '\r':
            read();
            if (peek(_cur, _end) == '\n') {
                read();
            }

            return std::nullopt;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            throw InvalidChar(_current_char, exceptionDetailInfo());
        default:
            if (c == 0) {
                throw InvalidChar(_current_char, exceptionDetailInfo());
            }
        }

        return read();
    }

    parser5::u8char parser5::hexEscape()
    {
        std::string buffer = "";
        auto        c = peek(_cur, _end);

        if (!unicode::isHexDigit(c)) {
            throw InvalidChar(_current_char, exceptionDetailInfo());
        }

        buffer += static_cast<char>(read());

        c = peek(_cur, _end);
        if (!unicode::isHexDigit(c)) {
            throw InvalidChar(_current_char, exceptionDetailInfo());
        }

        buffer += static_cast<char>(read());

        return std::stoi(buffer, nullptr, 16);
    }

    parser5::u8char parser5::unicodeEscape()
    {
        std::string buffer;
        int         count = 4;
        while (count-- > 0) {
            auto c = peek(_cur, _end);
            if (!unicode::isHexDigit(c)) {
                throw InvalidChar(_current_char, exceptionDetailInfo());
            }
            buffer += StringFromCharCode(read());
        }

        return std::stoull(buffer, nullptr, 16);
    }

    /* utf-8 reader */
    parser5::u8char parser5::peek(std::string::const_iterator& begin,
                                  const std::string::const_iterator& end,
                                  size_t* plen)
    {
        if (begin == end) {
            if (plen) *plen = 0;
            return 0;
        }
        uint8_t  head = (*begin);
        uint64_t ch = head;
        size_t   len = 1;
        while (begin != end && (head & 0b11000000) > 0b10000000) {
            head <<= 1;
            ++len;
            ch <<= 8;
            ch += (uint8_t)(*(begin + len - 1));
        }
        if (plen) {
            *plen = len;
        }
        return ch;
    }

    parser5::u8char parser5::peek(const std::string& str)
    {
        auto begin = str.begin();
        return peek(begin, str.cend());
    }
    constexpr size_t operator "" _sz(unsigned long long size)
    {
        return size;
    }
    parser5::u8char parser5::read()
    {
        size_t len = 0;
        _current_char = peek(_cur, _end, &len);
        if (_current_char == '\n') {
            _line++;
            _col = 0;
            _print_len = 0;
            _line_begin_cur = _cur + 1;
        }
        else if (_current_char > 0) {
            _col++;
        }

        if (len > 0) {
            _print_len += (std::min)(len, 2_sz);
            _cur += len;
        }
        return _current_char;
    }

    std::string parser5::StringFromCharCode(parser5::u8char code)
    {
        if (code == 0) return "";
        std::string str;
        for (auto i = 0; i < 8; ++i) {
            auto ch = (0xff & code);
            if (ch) str.insert(0, 1, static_cast<char>(ch));
            code >>= 8;
        }
        return str;
    }

    /* lex, parse, token */

    parser5::Token parser5::newToken(TokenType type, value value)
    {
        Token token;
        token.type = type;
        token._value = value;
        token.line = _line;
        token.col = _col;
        return token;
    }

    parser5::Token parser5::lex()
    {
        _lex_state = LexState::default_;
        _buffer = "";
        _double_quote = false;
        _sign = 1;

        std::optional<Token> token;

        for (;;) {
            _current_char = peek(_cur, _end);

            token = lexStates(_lex_state);
            if (token.has_value()) {
                return token.value();
            }
        }
    }

    std::optional<parser5::Token> parser5::lex_default()
    {
        switch (_current_char) {
        case '\t':
        case '\v':
        case '\f':
        case ' ':
        case '\n':
        case '\r':
        case 0x00A0:
        case 0xFEFF:
        case 0x2028:
        case 0x2029:
            read();
            return std::nullopt;
        case '/':
            read();
            _lex_state = LexState::comment;
            return std::nullopt;
        case 0:
            read();
            return newToken(TokenType::eof, value());
        }

        // if (_current_char.size() == 0) {
        //   read();
        //   return newToken("eof");
        // }

        if (unicode::isSpaceSeparator(_current_char)) {
            read();
            return std::nullopt;
        }

        //      throw InvalidLexState();
        return lexStates((LexState)_parse_state);
    }

    std::optional<parser5::Token> parser5::lex_comment()
    {
        switch (_current_char) {
        case '*':
            read();
            _lex_state = LexState::multiLineComment;
            return std::nullopt;

        case '/':
            read();
            _lex_state = LexState::singleLineComment;
            return std::nullopt;
        }

        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_multiLineComment()
    {
        if (_current_char == '*') {
            read();
            _lex_state = LexState::multiLineCommentAsterisk;
            return std::nullopt;
        }

        if (_current_char == 0) {
            throw InvalidChar(_current_char, exceptionDetailInfo());
        }

        read();
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_multiLineCommentAsterisk()
    {
        switch (_current_char) {
        case '*':
            read();
            return std::nullopt;

        case '/':
            read();
            _lex_state = LexState::default_;
            return std::nullopt;  //$

        default:
            if (_current_char == 0) {
                throw InvalidChar(_current_char, exceptionDetailInfo());
            }
        }

        read();
        _lex_state = LexState::multiLineComment;
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_singleLineComment()
    {
        switch (_current_char) {
        case '\n':
        case '\r':
        case 0x2028:
        case 0x2029:
            read();
            _lex_state = LexState::default_;
            return std::nullopt;
        case 0:
            read();
            return newToken(TokenType::eof, value());
        }
        read();
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_value()
    {
        switch (_current_char) {
        case '{':
        case '[':
            return newToken(TokenType::punctuator, value(StringFromCharCode(read())));
        case 'n':
            read();
            literal("ull");
            return newToken(TokenType::null, value());
        case 't':
            read();
            literal("rue");
            return newToken(TokenType::boolean, value(true));
        case 'f':
            read();
            literal("alse");
            return newToken(TokenType::boolean, value(false));
        case '-':
        case '+':
            if (read() == '-') {
                _sign = -1;
            }
            _lex_state = LexState::sign;
            return std::nullopt;
        case '.':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::decimalPointLeading;
            return std::nullopt;

        case '0':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::zero;
            return std::nullopt;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::decimalInteger;
            return std::nullopt;
        case 'I':
            read();
            literal("nfinity");
            return newToken(TokenType::numeric, INFINITY);
        case 'N':
            read();
            literal("aN");
            return newToken(TokenType::numeric, NAN);
        case '\"':
        case '\'':
            _double_quote = (read() == '\"');
            _buffer = "";
            _lex_state = LexState::string;
            return std::nullopt;
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_identifierNameStartEscape()
    {
        if (_current_char != 'u') {
            throw InvalidChar(_current_char, exceptionDetailInfo());
        }
        read();
        auto u = unicodeEscape();
        switch (u) {
        case '$':
        case '_':
            break;
        default:
            if (!unicode::isIdStartChar(u)) {
                throw InvalidIdentifier();
            }
            break;
        }
        _buffer += StringFromCharCode(u);
        _lex_state = LexState::identifierName;
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_identifierName()
    {
        switch (_current_char) {
        case '$':
        case '_':
        case 0x200C:
        case 0x200D:
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        case '\\':
            read();
            _lex_state = LexState::identifierNameEscape;
            return std::nullopt;
        }

        if (unicode::isIdContinueChar(_current_char)) {
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        }

        return newToken(TokenType::identifier, _buffer);
    }

    std::optional<parser5::Token> parser5::lex_identifierNameEscape()
    {
        if (_current_char != 'u') {
            throw InvalidChar(_current_char, exceptionDetailInfo());
        }
        read();
        auto u = unicodeEscape();
        switch (u) {
        case '$':
        case '_':
        case 0x200C:
        case 0x200D:
            break;
        default:
            if (!unicode::isIdStartChar(u)) {
                throw InvalidIdentifier();
            }
            break;
        }
        _buffer += StringFromCharCode(u);
        _lex_state = LexState::identifierName;
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_sign()
    {
        switch (_current_char) {
        case '.':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::decimalPointLeading;
            return std::nullopt;

        case '0':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::zero;
            return std::nullopt;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::decimalInteger;
            return std::nullopt;

        case 'I':
            read();
            literal("nfinity");
            return newToken(TokenType::numeric, _sign * INFINITY);

        case 'N':
            read();
            literal("aN");
            return newToken(TokenType::numeric, NAN);
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_zero()
    {
        switch (_current_char) {
        case '.':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalPoint;
            return std::nullopt;  //$

        case 'e':
        case 'E':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponent;
            return std::nullopt;  //$

        case 'x':
        case 'X':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::hexadecimal;
            return std::nullopt;  //$
        }
        return newToken(TokenType::numeric, _sign * 0);
    }

    std::optional<parser5::Token> parser5::lex_decimalInteger()
    {
        switch (_current_char) {
        case '.':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalPoint;
            return std::nullopt;
        case 'e':
        case 'E':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponent;
            return std::nullopt;  //$
        }
        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        }

        return newToken(TokenType::numeric, _sign * std::stod(_buffer));
    }

    std::optional<parser5::Token> parser5::lex_decimalPointLeading()
    {
        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalFraction;
            return std::nullopt;
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_decimalPoint()
    {
        switch (_current_char) {
        case 'e':
        case 'E':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponent;
            return std::nullopt;  //$
        }
        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalFraction;
            return std::nullopt;
        }

        return newToken(TokenType::numeric, _sign * std::stod(_buffer));
    }

    std::optional<parser5::Token> parser5::lex_decimalFraction()
    {
        switch (_current_char) {
        case 'e':
        case 'E':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponent;
            return std::nullopt;  //$
        }
        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        }

        return newToken(TokenType::numeric, _sign * std::stod(_buffer));
    }

    std::optional<parser5::Token> parser5::lex_decimalExponent()
    {
        switch (_current_char) {
        case '+':
        case '-':
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponentSign;
            return std::nullopt;  //$
        }

        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponentInteger;
            return std::nullopt;
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_decimalExponentSign()
    {
        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::decimalExponentInteger;
            return std::nullopt;
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_decimalExponentInteger()
    {
        if (unicode::isDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        }
        return newToken(TokenType::numeric, _sign * std::stod(_buffer));
    }

    std::optional<parser5::Token> parser5::lex_hexadecimal()
    {
        if (unicode::isHexDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::hexadecimalInteger;
            return std::nullopt;
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_hexdecimalInteger()
    {
        if (unicode::isHexDigit(_current_char)) {
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        }
        return newToken(TokenType::numeric, _sign * std::stod(_buffer));
    }

    std::optional<parser5::Token> parser5::lex_string()
    {
        switch (_current_char) {
        case '\\':
            read();
            _buffer += StringFromCharCode(escape().value_or(0));
            return std::nullopt;
        case '\"':
            if (_double_quote) {
                read();
                return newToken(TokenType::string, _buffer);
            }
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        case '\'':
            if (!_double_quote) {
                read();
                return newToken(TokenType::string, _buffer);
            }
            _buffer += StringFromCharCode(read());
            return std::nullopt;
        case '\n':
        case '\r':
            throw InvalidChar(_current_char, exceptionDetailInfo());
        case 0x2028:
        case 0x2029:
            // throw separatorChar(_current_char);
            break;
        default:
            if (_current_char == 0) {
                throw InvalidChar(_current_char, exceptionDetailInfo());
            }
        }
        _buffer += StringFromCharCode(read());
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_start()
    {
        switch (_current_char) {
        case '{':
        case '[':
            return newToken(TokenType::punctuator, StringFromCharCode(read()));
        }

        _lex_state = LexState::value;
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_beforePropertyName()
    {
        switch (_current_char) {
        case '$':
        case '_':
            _buffer = StringFromCharCode(read());
            _lex_state = LexState::identifierName;
            return std::nullopt;
        case '\\':
            read();
            _lex_state = LexState::identifierNameStartEscape;
            return std::nullopt;
        case '}':
            return newToken(TokenType::punctuator, StringFromCharCode(read()));
        case '\"':
        case '\'':
            _double_quote = (read() == '\"');
            _lex_state = LexState::string;
            return std::nullopt;
        }

        if (unicode::isIdStartChar(_current_char)) {
            _buffer += StringFromCharCode(read());
            _lex_state = LexState::identifierName;
            return std::nullopt;
        }

        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_afterPropertyName()
    {
        if (_current_char == ':') {
            return newToken(TokenType::punctuator, StringFromCharCode(read()));
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_beforePropertyValue()
    {
        _lex_state = LexState::value;
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_afterPropertyValue()
    {
        switch (_current_char) {
        case ',':
        case '}':
            return newToken(TokenType::punctuator, StringFromCharCode(read()));
        }
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_beforeArrayValue()
    {
        if (_current_char == ']') {
            return newToken(TokenType::punctuator, StringFromCharCode(read()));
        }
        _lex_state = LexState::value;
        return std::nullopt;
    }

    std::optional<parser5::Token> parser5::lex_afterArrayValue()
    {
        switch (_current_char) {
        case ',':
        case ']':
            return newToken(TokenType::punctuator, StringFromCharCode(read()));
        }

        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lex_end()
    {
        throw InvalidChar(_current_char, exceptionDetailInfo());
    }

    std::optional<parser5::Token> parser5::lexStates(LexState state)
    {
        switch (state) {
        case LexState::default_:
            return lex_default();
        case LexState::comment:
            return lex_comment();
        case LexState::multiLineComment:
            return lex_multiLineComment();
        case LexState::multiLineCommentAsterisk:
            return lex_multiLineCommentAsterisk();
        case LexState::singleLineComment:
            return lex_singleLineComment();
        case LexState::value:
            return lex_value();
        case LexState::identifierNameStartEscape:
            return lex_identifierNameStartEscape();
        case LexState::identifierName:
            return lex_identifierName();
        case LexState::identifierNameEscape:
            return lex_identifierNameEscape();
        case LexState::sign:
            return lex_sign();
        case LexState::zero:
            return lex_zero();
        case LexState::decimalInteger:
            return lex_decimalInteger();
        case LexState::decimalPointLeading:
            return lex_decimalPointLeading();
        case LexState::decimalPoint:
            return lex_decimalPoint();
        case LexState::decimalFraction:
            return lex_decimalFraction();
        case LexState::decimalExponent:
            return lex_decimalExponent();
        case LexState::decimalExponentSign:
            return lex_decimalExponentSign();
        case LexState::decimalExponentInteger:
            return lex_decimalExponentInteger();
        case LexState::hexadecimal:
            return lex_hexadecimal();
        case LexState::hexadecimalInteger:
            return lex_hexdecimalInteger();
        case LexState::string:
            return lex_string();
        case LexState::start:
            return lex_start();
        case LexState::beforePropertyName:
            return lex_beforePropertyName();
        case LexState::afterPropertyName:
            return lex_afterPropertyName();
        case LexState::beforePropertyValue:
            return lex_beforePropertyValue();
        case LexState::afterPropertyValue:
            return lex_afterPropertyValue();
        case LexState::beforeArrayValue:
            return lex_beforeArrayValue();
        case LexState::afterArrayValue:
            return lex_afterArrayValue();
        case LexState::end:
            return lex_end();
        }

        // throw
        return std::nullopt;
    }

    void parser5::parse_start()
    {
        if (token->type == TokenType::eof) {
            throw InvalidEOF();
        }

        push();
    }

    void parser5::parse_beforePropertyName()
    {
        switch (token->type) {
        case TokenType::identifier:
        case TokenType::string:
            key = token->_value.as_string();
            _parse_state = ParseState::afterPropertyName;
            break;
        case TokenType::punctuator:
            pop();
            break;
        case TokenType::eof:
            throw InvalidEOF();
        }
    }

    void parser5::parse_afterPropertyName()
    {
        if (token->type == TokenType::eof) {
            throw InvalidEOF();
        }

        _parse_state = ParseState::beforePropertyValue;
    }

    void parser5::parse_beforePropertyValue()
    {
        if (token->type == TokenType::eof) {
            throw InvalidEOF();
        }
        push();
    }

    void parser5::parse_beforeArrayValue()
    {
        if (token->type == TokenType::eof) {
            throw InvalidEOF();
        }

        if (token->type == TokenType::punctuator &&
            token->_value.as_string()[0] == ']') {
            pop();
            return;
        }

        push();
    }

    void parser5::parse_afterPropertyValue()
    {
        if (token->type == TokenType::eof) {
            throw InvalidEOF();
        }

        switch (token->_value.as_string()[0]) {
        case ',':
            _parse_state = ParseState::beforePropertyName;
            break;
        case '}':
            pop();
            break;
        }
    }

    void parser5::parse_afterArrayValue()
    {
        if (token->type == TokenType::eof) {
            throw InvalidEOF();
        }
        switch (token->_value.as_string()[0]) {
        case ',':
            _parse_state = ParseState::beforeArrayValue;
            break;
        case ']':
            pop();
            break;
        }
    }

    void parser5::parse_end() {}

    void parser5::parseStates(ParseState state)
    {
        switch (state) {
        case ParseState::start:
            parse_start();
            break;
        case ParseState::beforePropertyName:
            parse_beforePropertyName();
            break;
        case ParseState::afterPropertyName:
            parse_afterPropertyName();
            break;
        case ParseState::beforePropertyValue:
            parse_beforePropertyValue();
            break;
        case ParseState::beforeArrayValue:
            parse_beforeArrayValue();
            break;
        case ParseState::afterPropertyValue:
            parse_afterPropertyValue();
            break;
        case ParseState::afterArrayValue:
            parse_afterArrayValue();
            break;
        }
    }
    /* stack operation */
    void parser5::push()
    {
        value* v;
        switch (token->type) {
        case TokenType::punctuator:
            switch (token->_value.as_string()[0]) {
            case '{':
                v = new value(object());
                break;
            case '[':
                v = new value(array());
            }
            break;
        case TokenType::null:
        case TokenType::boolean:
        case TokenType::numeric:
        case TokenType::string:
            v = &token->_value;
            break;
        }
        if (!root.has_value()) {
            root = std::move(*v);
            v = &root.value();
        }
        else {
            auto parent = _stack.top();
            if (parent->is_array()) {
                parent->as_array().emplace_back(std::move(*v));
                v = &parent->as_array()[parent->as_array().size() - 1];
            }
            else {
                parent->as_object()[key] = std::move(*v);
                v = &parent->as_object()[key];
            }
        }

        if (v->is_object() || v->is_array()) {
            _stack.emplace(v);
            if (v->is_array()) {
                _parse_state = ParseState::beforeArrayValue;
            }
            else {
                _parse_state = ParseState::beforePropertyName;
            }
        }
        else {
            if (_stack.empty()) {
                _parse_state = ParseState::end;
            }
            else if (_stack.top()->is_array()) {
                _parse_state = ParseState::afterArrayValue;
            }
            else {
                _parse_state = ParseState::afterPropertyValue;
            }
        }
    }

    void parser5::pop()
    {
        _stack.pop();

        if (_stack.empty()) {
            _parse_state = ParseState::end;
        }
        else if (_stack.top()->is_array()) {
            _parse_state = ParseState::afterArrayValue;
        }
        else {
            _parse_state = ParseState::afterPropertyValue;
        }
    }

#endif  // JSON5
}  // namespace json
