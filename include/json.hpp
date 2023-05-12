#pragma once

#include <algorithm>
#include <fstream>
#include <initializer_list>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

#define MEOJSON_INLINE inline

namespace json
{
    template<typename StringType>
    class basic_value;
    template<typename StringType>
    class basic_array;
    template<typename StringType>
    class basic_object;

    using default_StringType = std::string;

    using value = typename basic_value<default_StringType>;
    using array = typename basic_array<default_StringType>;
    using object = typename basic_object<default_StringType>;

    // *************************
    // *     basic_value declare     *
    // *************************

    template<typename StringType>
    class basic_value
    {
        using array_ptr = typename std::unique_ptr<typename basic_array<StringType>>;
        using object_ptr = typename std::unique_ptr<typename basic_object<StringType>>;

    public:
        enum class value_type : char
        {
            Invalid,
            Null,
            Boolean,
            String,
            Number,
            Array,
            Object
        };

        using var_t = typename std::variant<StringType, array_ptr, object_ptr>;
        using char_t = typename StringType::value_type;
    public:
        basic_value();
        basic_value(const basic_value<StringType>& rhs);
        basic_value(basic_value<StringType>&& rhs) noexcept;

        basic_value(bool b);

        basic_value(int num);
        basic_value(unsigned num);
        basic_value(long num);
        basic_value(unsigned long num);
        basic_value(long long num);
        basic_value(unsigned long long num);
        basic_value(float num);
        basic_value(double num);
        basic_value(long double num);

        basic_value(const char_t* str);
        basic_value(StringType str);

        basic_value(basic_array<StringType> arr);
        // basic_value(std::initializer_list<typename basic_value> init_list); // for basic_array

        basic_value(basic_object<StringType> obj);
        // error: conversion from ‘<brace-enclosed initializer list>’ to ‘basic_value’
        // is ambiguous basic_value(std::initializer_list<std::pair<StringType, basic_value>>
        // init_list); // for basic_object

        // Constructed from raw data
        template <typename... Args>
        basic_value(value_type type, Args&&... args);

        // Prohibit conversion of other types to basic_value
        template <typename T>
        basic_value(T) = delete;

        ~basic_value();

        bool valid() const noexcept { return _type != value_type::Invalid; }
        bool empty() const noexcept { return is_null(); }
        bool is_null() const noexcept { return _type == value_type::Null; }
        bool is_number() const noexcept { return _type == value_type::Number; }
        bool is_boolean() const noexcept { return _type == value_type::Boolean; }
        bool is_string() const noexcept { return _type == value_type::String; }
        bool is_array() const noexcept { return _type == value_type::Array; }
        bool is_object() const noexcept { return _type == value_type::Object; }
        template <typename Type>
        bool is() const noexcept;

        bool contains(const StringType& key) const;
        bool contains(size_t pos) const;
        bool exists(const StringType& key) const { return contains(key); }
        bool exists(size_t pos) const { return contains(pos); }
        value_type type() const noexcept { return _type; }
        const basic_value<StringType>& at(size_t pos) const;
        const basic_value<StringType>& at(const StringType& key) const;

        // usage: get(key, key_child, ..., default_value);
        template <typename... KeysThenDefaultValue>
        decltype(auto) get(KeysThenDefaultValue&&... keys_then_default_value) const;

        template <typename Type = basic_value<StringType>>
        std::optional<Type> find(size_t pos) const;
        template <typename Type = basic_value<StringType>>
        std::optional<Type> find(const StringType& key) const;

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
        const StringType as_string() const;
        const basic_array<StringType>& as_array() const;
        const basic_object<StringType>& as_object() const;
        template <typename Type>
        Type as() const;

        basic_array<StringType>& as_array();
        basic_object<StringType>& as_object();

        template <typename... Args>
        decltype(auto) array_emplace(Args&&... args);
        template <typename... Args>
        decltype(auto) object_emplace(Args&&... args);
        void clear() noexcept;

        // return raw string
        const StringType to_string() const;
        const StringType format(bool ordered = false, StringType shift_str = "    ",
                                 size_t basic_shift_count = 0) const;

        basic_value<StringType>& operator=(const basic_value<StringType>& rhs);
        basic_value<StringType>& operator=(basic_value<StringType>&&) noexcept;

        bool operator==(const basic_value<StringType>& rhs) const;
        bool operator!=(const basic_value<StringType>& rhs) const { return !(*this == rhs); }

        const basic_value<StringType>& operator[](size_t pos) const;
        basic_value<StringType>& operator[](size_t pos);
        basic_value<StringType>& operator[](const StringType& key);
        basic_value<StringType>& operator[](StringType&& key);

        basic_value<StringType> operator|(const basic_object<StringType>& rhs)&;
        basic_value<StringType> operator|(basic_object<StringType>&& rhs)&;
        basic_value<StringType> operator|(const basic_object<StringType>& rhs)&&;
        basic_value<StringType> operator|(basic_object<StringType>&& rhs)&&;

        basic_value<StringType>& operator|=(const basic_object<StringType>& rhs);
        basic_value<StringType>& operator|=(basic_object<StringType>&& rhs);

        basic_value<StringType> operator+(const basic_array<StringType>& rhs)&;
        basic_value<StringType> operator+(basic_array<StringType>&& rhs)&;
        basic_value<StringType> operator+(const basic_array<StringType>& rhs)&&;
        basic_value<StringType> operator+(basic_array<StringType>&& rhs)&&;

        basic_value<StringType>& operator+=(const basic_array<StringType>& rhs);
        basic_value<StringType>& operator+=(basic_array<StringType>&& rhs);

        explicit operator bool() const { return as_boolean(); }
        explicit operator int() const { return as_integer(); }
        explicit operator long() const { return as_long(); }
        explicit operator unsigned long() const { return as_unsigned_long(); }
        explicit operator long long() const { return as_long_long(); }
        explicit operator unsigned long long() const { return as_unsigned_long_long(); }
        explicit operator float() const { return as_float(); }
        explicit operator double() const { return as_double(); }
        explicit operator long double() const { return as_long_double(); }
        explicit operator StringType() const { return as_string(); }

    private:
        static var_t deep_copy(const var_t& src);

        template <typename... KeysThenDefaultValue, size_t... KeysIndexes>
        decltype(auto) get(std::tuple<KeysThenDefaultValue...> keys_then_default_value,
                           std::index_sequence<KeysIndexes...>) const;

        template <typename T, typename FirstKey, typename... RestKeys>
        decltype(auto) get_aux(T&& default_value, FirstKey&& first, RestKeys&&... rest) const;
        template <typename T, typename UniqueKey>
        decltype(auto) get_aux(T&& default_value, UniqueKey&& first) const;

        const StringType& as_basic_type_str() const;
        StringType& as_basic_type_str();

        value_type _type = value_type::Null;
        var_t _raw_data;
    };

    template<typename StringType = default_StringType>
    const basic_value<StringType> invalid_value();
    template<typename StringType>
    std::ostream& operator<<(std::ostream& out, const basic_value<StringType>& val);

    // *************************
    // *     basic_array declare     *
    // *************************
    template<typename StringType>
    class basic_array
    {
    public:
        using raw_array = typename std::vector<typename basic_value<StringType>>;
        using value_type = typename raw_array::value_type;
        using iterator = typename raw_array::iterator;
        using const_iterator = typename raw_array::const_iterator;
        using reverse_iterator = typename raw_array::reverse_iterator;
        using const_reverse_iterator = typename raw_array::const_reverse_iterator;
        using char_t = typename StringType::value_type;

    public:
        basic_array() = default;
        basic_array(const basic_array<StringType>& rhs) = default;
        basic_array(basic_array<StringType>&& rhs) noexcept = default;
        basic_array(const raw_array& arr);
        basic_array(raw_array&& arr) noexcept;
        basic_array(std::initializer_list<typename raw_array::value_type> init_list);
        basic_array(typename raw_array::size_type size);

        explicit basic_array(const basic_value<StringType>& val);
        explicit basic_array(basic_value<StringType>&& val);
        template <typename ArrayType,
            typename EnableT = std::enable_if_t<std::is_constructible_v<typename basic_value<StringType>, typename ArrayType::value_type>>>
            basic_array(ArrayType arr);

        ~basic_array() noexcept = default;

        bool empty() const noexcept { return _array_data.empty(); }
        size_t size() const noexcept { return _array_data.size(); }
        bool contains(size_t pos) const { return pos < _array_data.size(); }
        bool exists(size_t pos) const { return contains(pos); }
        const basic_value<StringType>& at(size_t pos) const;
        const StringType to_string() const;
        const StringType format(bool ordered = false, StringType shift_str = "    ",
                                 size_t basic_shift_count = 0) const;

        bool get(size_t pos, bool default_value) const;
        int get(size_t pos, int default_value) const;
        long get(size_t pos, long default_value) const;
        unsigned long get(size_t pos, unsigned default_value) const;
        long long get(size_t pos, long long default_value) const;
        unsigned long long get(size_t pos, unsigned long long default_value) const;
        float get(size_t pos, float default_value) const;
        double get(size_t pos, double default_value) const;
        long double get(size_t pos, long double default_value) const;
        const StringType get(size_t pos, StringType default_value) const;
        const StringType get(size_t pos, const char_t* default_value) const;
        const basic_value<StringType>& get(size_t pos) const;

        template <typename Type = basic_value<StringType>>
        std::optional<Type> find(size_t pos) const;

        template <typename... Args>
        decltype(auto) emplace_back(Args&&... args);

        void clear() noexcept;
        // void erase(size_t pos);

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

        const basic_value<StringType>& operator[](size_t pos) const;
        basic_value<StringType>& operator[](size_t pos);

        basic_array<StringType> operator+(const basic_array<StringType>& rhs)&;
        basic_array<StringType> operator+(basic_array<StringType>&& rhs)&;
        basic_array<StringType> operator+(const basic_array<StringType>& rhs)&&;
        basic_array<StringType> operator+(basic_array<StringType>&& rhs)&&;

        basic_array<StringType>& operator+=(const basic_array<StringType>& rhs);
        basic_array<StringType>& operator+=(basic_array<StringType>&& rhs);

        basic_array<StringType>& operator=(const basic_array<StringType>&) = default;
        basic_array<StringType>& operator=(basic_array<StringType>&&) noexcept = default;

        bool operator==(const basic_array<StringType>& rhs) const;
        bool operator!=(const basic_array<StringType>& rhs) const { return !(*this == rhs); }

        // const raw_array &raw_data() const;

    private:
        raw_array _array_data;
    };

    template<typename StringType>
    std::ostream& operator<<(std::ostream& out, const basic_array<StringType>& arr);

    // *************************
    // *     basic_object declare    *
    // *************************
    template<typename StringType>
    class basic_object
    {
    public:
        using raw_object = typename std::unordered_map<StringType, typename basic_value<StringType>>;
        using value_type = typename raw_object::value_type;
        using iterator = typename raw_object::iterator;
        using const_iterator = typename raw_object::const_iterator;
        using char_t = typename StringType::value_type;

    public:
        basic_object() = default;
        basic_object(const basic_object<StringType>& rhs) = default;
        basic_object(basic_object<StringType>&& rhs) noexcept = default;
        basic_object(const raw_object& raw_obj);
        basic_object(raw_object&& raw_obj);
        basic_object(std::initializer_list<value_type> init_list);
        explicit basic_object(const basic_value<StringType>& val);
        explicit basic_object(basic_value<StringType>&& val);
        template <typename MapType, typename EnableT = std::enable_if_t<
            std::is_constructible_v<value_type, typename MapType::value_type>>>
            basic_object(MapType map);

        ~basic_object() = default;

        bool empty() const noexcept { return _object_data.empty(); }
        size_t size() const noexcept { return _object_data.size(); }
        bool contains(const StringType& key) const;
        bool exists(const StringType& key) const { return contains(key); }
        const basic_value<StringType>& at(const StringType& key) const;
        const StringType to_string() const;
        const StringType format(bool ordered = false, StringType shift_str = "    ",
                                 size_t basic_shift_count = 0) const;

        bool get(const StringType& key, bool default_value) const;
        int get(const StringType& key, int default_value) const;
        long get(const StringType& key, long default_value) const;
        unsigned long get(const StringType& key, unsigned default_value) const;
        long long get(const StringType& key, long long default_value) const;
        unsigned long long get(const StringType& key, unsigned long long default_value) const;
        float get(const StringType& key, float default_value) const;
        double get(const StringType& key, double default_value) const;
        long double get(const StringType& key, long double default_value) const;
        const StringType get(const StringType& key, StringType default_value) const;
        const StringType get(const StringType& key, const char_t* default_value) const;
        const basic_value<StringType>& get(const StringType& key) const;

        template <typename Type = basic_value<StringType>>
        std::optional<Type> find(const StringType& key) const;

        template <typename... Args>
        decltype(auto) emplace(Args&&... args);
        template <typename... Args>
        decltype(auto) insert(Args&&... args);

        void clear() noexcept;
        bool erase(const StringType& key);

        iterator begin() noexcept;
        iterator end() noexcept;
        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;
        const_iterator cbegin() const noexcept;
        const_iterator cend() const noexcept;

        basic_value<StringType>& operator[](const StringType& key);
        basic_value<StringType>& operator[](StringType&& key);

        basic_object<StringType> operator|(const basic_object<StringType>& rhs)&;
        basic_object<StringType> operator|(basic_object<StringType>&& rhs)&;
        basic_object<StringType> operator|(const basic_object<StringType>& rhs)&&;
        basic_object<StringType> operator|(basic_object<StringType>&& rhs)&&;

        basic_object<StringType>& operator|=(const basic_object<StringType>& rhs);
        basic_object<StringType>& operator|=(basic_object<StringType>&& rhs);

        basic_object<StringType>& operator=(const basic_object<StringType>&) = default;
        basic_object<StringType>& operator=(basic_object<StringType>&&) = default;

        bool operator==(const basic_object<StringType>& rhs) const;
        bool operator!=(const basic_object<StringType>& rhs) const { return !(*this == rhs); }

        // const raw_object &raw_data() const;

    private:
        raw_object _object_data;
    };

    template<typename StringType>
    std::ostream& operator<<(std::ostream& out, const basic_object<StringType>& obj);

    // *************************
    // *   exception declare   *
    // *************************
    class exception : public std::exception
    {
    public:
        exception() = default;
        exception(const std::string& msg) : _what(msg) {}

        exception(const exception&) = default;
        exception& operator=(const exception&) = default;
        exception(exception&&) = default;
        exception& operator=(exception&&) = default;

        virtual ~exception() noexcept override = default;

        virtual const char* what() const noexcept override
        {
            return _what.empty() ? "Unknown exception" : _what.c_str();
        }

    protected:
        std::string _what;
    };

    // *************************
    // *      aux declare      *
    // *************************
    template<typename StringType>
    StringType unescape_string(StringType str);
    template<typename StringType>
    StringType escape_string(StringType str);

    // *************************
    // *       basic_value<StringType> impl      *
    // *************************
    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value() = default;

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(const basic_value<StringType>& rhs) : _type(rhs._type), _raw_data(deep_copy(rhs._raw_data))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(basic_value<StringType>&& rhs) noexcept = default;

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(bool b) : _type(value_type::Boolean), _raw_data(StringType(b ? "true" : "false"))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(int num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(unsigned num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(long num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(unsigned long num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(long long num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(unsigned long long num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(float num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(double num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(long double num) : _type(value_type::Number), _raw_data(std::to_string(num))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(const char_t* str) : _type(value_type::String), _raw_data(unescape_string(StringType(str)))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(StringType str) : _type(value_type::String), _raw_data(unescape_string(std::move(str)))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(basic_array<StringType> arr)
        : _type(value_type::Array), _raw_data(std::make_unique<basic_array<StringType>>(std::move(arr)))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::basic_value(basic_object<StringType> obj)
        : _type(value_type::Object), _raw_data(std::make_unique<basic_object<StringType>>(std::move(obj)))
    {
        ;
    }

    // for Pimpl
    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>::~basic_value() = default;

    template<typename StringType>
    template<typename Type>
    MEOJSON_INLINE bool basic_value<StringType>::is() const noexcept
    {
        if constexpr (std::is_same_v<Type, basic_value<StringType>>) {
            return true;
        }
        else if constexpr (std::is_same_v<Type, bool>) {
            return _type == value_type::Boolean;
        }
        else if constexpr (std::is_same_v<Type, int> || std::is_same_v<Type, unsigned> || std::is_same_v<Type, long> ||
                           std::is_same_v<Type, unsigned long> || std::is_same_v<Type, long long> ||
                           std::is_same_v<Type, unsigned long long> || std::is_same_v<Type, float> ||
                           std::is_same_v<Type, double> || std::is_same_v<Type, long double>) {
            return _type == value_type::Number;
        }
        else if constexpr (std::is_same_v<Type, StringType>) {
            return _type == value_type::String;
        }
        else if constexpr (std::is_same_v<Type, basic_array<StringType>>) {
            return _type == value_type::Array;
        }
        else if constexpr (std::is_same_v<Type, basic_object<StringType>>) {
            return _type == value_type::Object;
        }
        else {
            static_assert(!sizeof(Type), "Unsupported type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE bool basic_value<StringType>::contains(const StringType& key) const
    {
        return is_object() && as_object().contains(key);
    }

    template<typename StringType>
    MEOJSON_INLINE bool basic_value<StringType>::contains(size_t pos) const
    {
        return is_array() && as_array().contains(pos);
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_value<StringType>::at(size_t pos) const
    {
        return as_array().at(pos);
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_value<StringType>::at(const StringType& key) const
    {
        return as_object().at(key);
    }

    template<typename StringType>
    template <typename... KeysThenDefaultValue>
    MEOJSON_INLINE decltype(auto) basic_value<StringType>::get(KeysThenDefaultValue&&... keys_then_default_value) const
    {
        return get(std::forward_as_tuple(keys_then_default_value...),
                   std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
    }

    template<typename StringType>
    template <typename... KeysThenDefaultValue, size_t... KeysIndexes>
    MEOJSON_INLINE decltype(auto) basic_value<StringType>::get(std::tuple<KeysThenDefaultValue...> keys_then_default_value,
                                             std::index_sequence<KeysIndexes...>) const
    {
        constexpr unsigned long DefaultValueIndex = sizeof...(KeysThenDefaultValue) - 1;
        return get_aux(std::get<DefaultValueIndex>(keys_then_default_value),
                       std::get<KeysIndexes>(keys_then_default_value)...);
    }

    template<typename StringType>
    template <typename T, typename FirstKey, typename... RestKeys>
    MEOJSON_INLINE decltype(auto) basic_value<StringType>::get_aux(T&& default_value, FirstKey&& first, RestKeys&&... rest) const
    {
        if constexpr (std::is_constructible_v<StringType, FirstKey>) {
            return is_object() ? as_object()
                .get(std::forward<FirstKey>(first))
                .get_aux(std::forward<T>(default_value), std::forward<RestKeys>(rest)...)
                : default_value;
        }
        else if constexpr (std::is_integral_v<typename std::remove_reference<FirstKey>::type>) {
            return is_array() ? as_array()
                .get(std::forward<FirstKey>(first))
                .get_aux(std::forward<T>(default_value), std::forward<RestKeys>(rest)...)
                : default_value;
        }
        else {
            static_assert(!sizeof(FirstKey), "Parameter must be integral or StringType constructible");
        }
    }

    template<typename StringType>
    template <typename T, typename UniqueKey>
    MEOJSON_INLINE decltype(auto) basic_value<StringType>::get_aux(T&& default_value, UniqueKey&& first) const
    {
        if constexpr (std::is_constructible_v<StringType, UniqueKey>) {
            return is_object() ? as_object().get(std::forward<UniqueKey>(first), std::forward<T>(default_value))
                : default_value;
        }
        else if constexpr (std::is_integral_v<typename std::remove_reference<UniqueKey>::type>) {
            return is_array() ? as_array().get(std::forward<UniqueKey>(first), std::forward<T>(default_value))
                : default_value;
        }
        else {
            static_assert(!sizeof(UniqueKey), "Parameter must be integral or StringType constructible");
        }
    }

    template<typename StringType>
    template <typename Type>
    MEOJSON_INLINE std::optional<Type> basic_value<StringType>::find(size_t pos) const
    {
        return is_array() ? as_array().template find<Type>(pos) : std::nullopt;
    }

    template<typename StringType>
    template <typename Type>
    MEOJSON_INLINE std::optional<Type> basic_value<StringType>::find(const StringType& key) const
    {
        return is_object() ? as_object().template find<Type>(key) : std::nullopt;
    }

    template<typename StringType>
    MEOJSON_INLINE bool basic_value<StringType>::as_boolean() const
    {
        if (is_boolean()) {
            if (const StringType& b_str = as_basic_type_str(); b_str == "true") {
                return true;
            }
            else if (b_str == "false") {
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

    template<typename StringType>
    MEOJSON_INLINE int basic_value<StringType>::as_integer() const
    {
        if (is_number()) {
            return std::stoi(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long basic_value<StringType>::as_long() const
    {
        if (is_number()) {
            return std::stol(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE unsigned long basic_value<StringType>::as_unsigned_long() const
    {
        if (is_number()) {
            return std::stoul(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long long basic_value<StringType>::as_long_long() const
    {
        if (is_number()) {
            return std::stoll(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE unsigned long long basic_value<StringType>::as_unsigned_long_long() const
    {
        if (is_number()) {
            return std::stoull(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE float basic_value<StringType>::as_float() const
    {
        if (is_number()) {
            return std::stof(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE double basic_value<StringType>::as_double() const
    {
        if (is_number()) {
            return std::stod(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long double basic_value<StringType>::as_long_double() const
    {
        if (is_number()) {
            return std::stold(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_value<StringType>::as_string() const
    {
        if (is_string()) {
            return escape_string(as_basic_type_str());
        }
        else {
            throw exception("Wrong Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_array<StringType>& basic_value<StringType>::as_array() const
    {
        if (is_array()) {
            return *std::get<array_ptr>(_raw_data);
        }

        throw exception("Wrong Type");
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_object<StringType>& basic_value<StringType>::as_object() const
    {
        if (is_object()) {
            return *std::get<object_ptr>(_raw_data);
        }

        throw exception("Wrong Type or data empty");
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>& basic_value<StringType>::as_array()
    {
        if (empty()) {
            *this = basic_array<StringType>();
        }

        if (is_array()) {
            return *std::get<array_ptr>(_raw_data);
        }

        throw exception("Wrong Type");
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>& basic_value<StringType>::as_object()
    {
        if (empty()) {
            *this = basic_object<StringType>();
        }

        if (is_object()) {
            return *std::get<object_ptr>(_raw_data);
        }

        throw exception("Wrong Type or data empty");
    }

    template<typename StringType>
    template <typename Type>
    MEOJSON_INLINE Type basic_value<StringType>::as() const
    {
        return static_cast<Type>(*this);
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType& basic_value<StringType>::as_basic_type_str() const
    {
        return std::get<StringType>(_raw_data);
    }

    template<typename StringType>
    MEOJSON_INLINE StringType& basic_value<StringType>::as_basic_type_str()
    {
        return std::get<StringType>(_raw_data);
    }

    template<typename StringType>
    template <typename... Args>
    MEOJSON_INLINE decltype(auto) basic_value<StringType>::array_emplace(Args&&... args)
    {
        return as_array().emplace_back(std::forward<Args>(args)...);
    }

    template<typename StringType>
    template <typename... Args>
    MEOJSON_INLINE decltype(auto) basic_value<StringType>::object_emplace(Args&&... args)
    {
        return as_object().emplace(std::forward<Args>(args)...);
    }

    template<typename StringType>
    MEOJSON_INLINE void basic_value<StringType>::clear() noexcept
    {
        *this = basic_value<StringType>();
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_value<StringType>::to_string() const
    {
        switch (_type) {
        case value_type::Null:
            return "null";
        case value_type::Boolean:
        case value_type::Number:
            return as_basic_type_str();
        case value_type::String:
            return '"' + as_basic_type_str() + '"';
        case value_type::Array:
            return as_array().to_string();
        case value_type::Object:
            return as_object().to_string();
        default:
            throw exception("Unknown basic_value Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_value<StringType>::format(bool ordered, StringType shift_str, size_t basic_shift_count) const
    {
        switch (_type) {
        case value_type::Null:
            return "null";
        case value_type::Boolean:
        case value_type::Number:
            return as_basic_type_str();
        case value_type::String:
            return '"' + as_basic_type_str() + '"';
        case value_type::Array:
            return as_array().format(ordered, shift_str, basic_shift_count);
        case value_type::Object:
            return as_object().format(ordered, shift_str, basic_shift_count);
        default:
            throw exception("Unknown basic_value Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator=(const basic_value<StringType>& rhs)
    {
        _type = rhs._type;
        _raw_data = deep_copy(rhs._raw_data);

        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator=(basic_value<StringType>&& rhs) noexcept = default;

    template<typename StringType>
    MEOJSON_INLINE bool basic_value<StringType>::operator==(const basic_value<StringType>& rhs) const
    {
        if (_type != rhs._type) return false;

        switch (_type) {
        case value_type::Null:
            return rhs.is_null();
        case value_type::Boolean:
        case value_type::Number:
        case value_type::String:
            return _raw_data == rhs._raw_data;
        case value_type::Array:
            return as_array() == rhs.as_array();
        case value_type::Object:
            return as_object() == rhs.as_object();
        default:
            throw exception("Unknown basic_value Type");
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_value<StringType>::operator[](size_t pos) const
    {
        // basic_array not support to create by operator[]

        return as_array()[pos];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator[](size_t pos)
    {
        // basic_array not support to create by operator[]

        return as_array()[pos];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator[](const StringType& key)
    {
        if (empty()) {
            *this = basic_object<StringType>();
        }

        return as_object()[key];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator[](StringType&& key)
    {
        if (empty()) {
            *this = basic_object<StringType>();
        }

        return as_object()[std::move(key)];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator|(const basic_object<StringType>& rhs)&
    {
        return as_object() | rhs;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator|(basic_object<StringType>&& rhs)&
    {
        return as_object() | std::move(rhs);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator|(const basic_object<StringType>& rhs)&&
    {
        return std::move(as_object()) | rhs;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator|(basic_object<StringType>&& rhs)&&
    {
        return std::move(as_object()) | std::move(rhs);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator|=(const basic_object<StringType>& rhs)
    {
        as_object() |= rhs;
        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator|=(basic_object<StringType>&& rhs)
    {
        as_object() |= std::move(rhs);
        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator+(const basic_array<StringType>& rhs)&
    {
        return as_array() + rhs;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator+(basic_array<StringType>&& rhs)&
    {
        return as_array() + std::move(rhs);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator+(const basic_array<StringType>& rhs)&&
    {
        return std::move(as_array()) + rhs;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType> basic_value<StringType>::operator+(basic_array<StringType>&& rhs)&&
    {
        return std::move(as_array()) + std::move(rhs);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator+=(const basic_array<StringType>& rhs)
    {
        as_array() += rhs;
        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_value<StringType>::operator+=(basic_array<StringType>&& rhs)
    {
        as_array() += std::move(rhs);
        return *this;
    }

    template<typename StringType>
    template <typename... Args>
    basic_value<StringType>::basic_value(value_type type, Args&&... args) : _type(type), _raw_data(std::forward<Args>(args)...)
    {
        static_assert(std::is_constructible_v<var_t, Args...>, "Parameter can't be used to construct a var_t");
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_value<StringType>::var_t basic_value<StringType>::deep_copy(const var_t& src)
    {
        var_t dst;
        if (const auto string_ptr = std::get_if<StringType>(&src)) {
            dst = *string_ptr;
        }
        else if (const auto arr_ptr = std::get_if<array_ptr>(&src)) {
            dst = std::make_unique<basic_array<StringType>>(**arr_ptr);
        }
        else if (const auto obj_ptr = std::get_if<object_ptr>(&src)) {
            dst = std::make_unique<basic_object<StringType>>(**obj_ptr);
        }
        else {
            // maybe invalid_value
        }

        return dst;
    }

    // *************************
    // *       basic_array impl      *
    // *************************
    template<typename StringType>
    template <typename... Args>
    decltype(auto) basic_array<StringType>::emplace_back(Args&&... args)
    {
        static_assert(std::is_constructible_v<value_type, Args...>,
                      "Parameter can't be used to construct a raw_array::value_type");
        return _array_data.emplace_back(std::forward<Args>(args)...);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>::basic_array(const raw_array& arr) : _array_data(arr)
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>::basic_array(raw_array&& arr) noexcept : _array_data(std::move(arr))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE
        basic_array<StringType>::basic_array(std::initializer_list<value_type> init_list) : _array_data(init_list)
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE
        basic_array<StringType>::basic_array(typename raw_array::size_type size) : _array_data(size)
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>::basic_array(const basic_value<StringType>& val) : basic_array<StringType>(val.as_array())
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>::basic_array(basic_value<StringType>&& val) : basic_array<StringType>(std::move(val.as_array()))
    {
        ;
    }

    template<typename StringType>
    template <typename ArrayType, typename EnableT>
    MEOJSON_INLINE basic_array<StringType>::basic_array(ArrayType arr)
    {
        _array_data.assign(std::make_move_iterator(arr.begin()), std::make_move_iterator(arr.end()));
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_array<StringType>::at(size_t pos) const
    {
        return _array_data.at(pos);
    }

    template<typename StringType>
    MEOJSON_INLINE void basic_array<StringType>::clear() noexcept
    {
        _array_data.clear();
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_array<StringType>::to_string() const
    {
        StringType str = "[";
        for (const basic_value<StringType>& val : _array_data) {
            str += val.to_string() + ",";
        }
        if (str.back() == ',') {
            str.pop_back();
        }
        str += "]";
        return str;
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_array<StringType>::format(bool ordered, StringType shift_str, size_t basic_shift_count) const
    {
        StringType shift;
        for (size_t i = 0; i != basic_shift_count + 1; ++i) {
            shift += shift_str;
        }

        StringType str = "[";
        for (const basic_value<StringType>& val : _array_data) {
            str += "\n" + shift + val.format(ordered, shift_str, basic_shift_count + 1) + ",";
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

    template<typename StringType>
    MEOJSON_INLINE bool basic_array<StringType>::get(size_t pos, bool default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_boolean()) {
                return basic_value.as_boolean();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE int basic_array<StringType>::get(size_t pos, int default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_integer();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long basic_array<StringType>::get(size_t pos, long default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE unsigned long basic_array<StringType>::get(size_t pos, unsigned default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_unsigned_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long long basic_array<StringType>::get(size_t pos, long long default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE unsigned long long basic_array<StringType>::get(size_t pos, unsigned long long default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_unsigned_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE float basic_array<StringType>::get(size_t pos, float default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_float();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE double basic_array<StringType>::get(size_t pos, double default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long double basic_array<StringType>::get(size_t pos, long double default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_number()) {
                return basic_value.as_long_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_array<StringType>::get(size_t pos, StringType default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_string()) {
                return basic_value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_array<StringType>::get(size_t pos, const char_t* default_value) const
    {
        if (contains(pos)) {
            basic_value<StringType> basic_value = _array_data.at(pos);
            if (basic_value.is_string()) {
                return basic_value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_array<StringType>::get(size_t pos) const
    {
        if (contains(pos)) {
            return _array_data.at(pos);
        }
        else {
            static basic_value<StringType> null;
            return null;
        }
    }

    template<typename StringType>
    template <typename Type>
    MEOJSON_INLINE std::optional<Type> basic_array<StringType>::find(size_t pos) const
    {
        static_assert(std::is_constructible_v<Type, basic_value<StringType>>, "Type can NOT be constructed by basic_value");
        if (!contains(pos)) {
            return std::nullopt;
        }
        const auto& val = _array_data.at(pos);
        return val.is<Type>() ? std::optional<Type>(val.as<Type>()) : std::nullopt;
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::iterator basic_array<StringType>::begin() noexcept
    {
        return _array_data.begin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::iterator basic_array<StringType>::end() noexcept
    {
        return _array_data.end();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_iterator basic_array<StringType>::begin() const noexcept
    {
        return _array_data.begin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_iterator basic_array<StringType>::end() const noexcept
    {
        return _array_data.end();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_iterator basic_array<StringType>::cbegin() const noexcept
    {
        return _array_data.cbegin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_iterator basic_array<StringType>::cend() const noexcept
    {
        return _array_data.cend();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::reverse_iterator basic_array<StringType>::rbegin() noexcept
    {
        return _array_data.rbegin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::reverse_iterator basic_array<StringType>::rend() noexcept
    {
        return _array_data.rend();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_reverse_iterator basic_array<StringType>::rbegin() const noexcept
    {
        return _array_data.rbegin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_reverse_iterator basic_array<StringType>::rend() const noexcept
    {
        return _array_data.rend();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_reverse_iterator basic_array<StringType>::crbegin() const noexcept
    {
        return _array_data.crbegin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_array<StringType>::const_reverse_iterator basic_array<StringType>::crend() const noexcept
    {
        return _array_data.crend();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_value<StringType>& basic_array<StringType>::operator[](size_t pos)
    {
        return _array_data[pos];
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_array<StringType>::operator[](size_t pos) const
    {
        return _array_data[pos];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType> basic_array<StringType>::operator+(const basic_array<StringType>& rhs)&
    {
        basic_array<StringType> temp = *this;
        temp._array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
        return temp;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType> basic_array<StringType>::operator+(basic_array<StringType>&& rhs)&
    {
        basic_array<StringType> temp = *this;
        temp._array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()),
                                std::make_move_iterator(rhs.end()));
        return temp;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType> basic_array<StringType>::operator+(const basic_array<StringType>& rhs)&&
    {
        _array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
        return std::move(*this);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType> basic_array<StringType>::operator+(basic_array<StringType>&& rhs)&&
    {
        _array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
        return std::move(*this);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>& basic_array<StringType>::operator+=(const basic_array<StringType>& rhs)
    {
        _array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_array<StringType>& basic_array<StringType>::operator+=(basic_array<StringType>&& rhs)
    {
        _array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE bool basic_array<StringType>::operator==(const basic_array<StringType>& rhs) const
    {
        return _array_data == rhs._array_data;
    }

    // const raw_array &basic_array<StringType>::raw_data() const
    // {
    //     return _array_data;
    // }

    // *************************
    // *      basic_object impl      *
    // *************************
    template<typename StringType>
    template <typename... Args>
    decltype(auto) basic_object<StringType>::emplace(Args&&... args)
    {
        static_assert(std::is_constructible_v<value_type, Args...>,
                      "Parameter can't be used to construct a raw_object::value_type");
        return _object_data.emplace(std::forward<Args>(args)...);
    }

    template<typename StringType>
    template <typename... Args>
    decltype(auto) basic_object<StringType>::insert(Args&&... args)
    {
        return _object_data.insert(std::forward<Args>(args)...);
    }

    template<typename StringType>
    MEOJSON_INLINE std::ostream& operator<<(std::ostream& out, const basic_array<StringType>& arr)
    {
        // TODO: format output

        out << arr.to_string();
        return out;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>::basic_object(const raw_object& raw_obj) : _object_data(raw_obj)
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>::basic_object(raw_object&& raw_obj) : _object_data(std::move(raw_obj))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE
        basic_object<StringType>::basic_object(std::initializer_list<typename raw_object::value_type> init_list)
    {
        _object_data.reserve(init_list.size());
        for (const auto& [key, val] : init_list) {
            emplace(key, val);
        }
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>::basic_object(const basic_value<StringType>& val) : basic_object<StringType>(val.as_object())
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>::basic_object(basic_value<StringType>&& val) : basic_object<StringType>(std::move(val.as_object()))
    {
        ;
    }

    template<typename StringType>
    MEOJSON_INLINE bool basic_object<StringType>::contains(const StringType& key) const
    {
        return _object_data.find(key) != _object_data.cend();
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_object<StringType>::at(const StringType& key) const
    {
        return _object_data.at(key);
    }

    template<typename StringType>
    MEOJSON_INLINE void basic_object<StringType>::clear() noexcept
    {
        _object_data.clear();
    }

    template<typename StringType>
    MEOJSON_INLINE bool basic_object<StringType>::erase(const StringType& key)
    {
        return _object_data.erase(key) > 0 ? true : false;
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_object<StringType>::to_string() const
    {
        StringType str = "{";
        for (const auto& [key, val] : _object_data) {
            str += "\"" + unescape_string(key) + "\":" + val.to_string() + ",";
        }
        if (str.back() == ',') {
            str.pop_back();
        }
        str += "}";
        return str;
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_object<StringType>::format(bool ordered, StringType shift_str, size_t basic_shift_count) const
    {
        StringType shift;
        for (size_t i = 0; i != basic_shift_count + 1; ++i) {
            shift += shift_str;
        }

        StringType str = "{";
        auto append_kv = [&](const StringType& key, const basic_value<StringType>& val) {
            str += "\n" + shift + "\"" + unescape_string(key) +
                "\": " + val.format(ordered, shift_str, basic_shift_count + 1) + ",";
        };

        if (ordered) {
            std::vector<raw_object::const_iterator> ordered_data;
            for (auto it = _object_data.cbegin(); it != _object_data.cend(); ++it) {
                ordered_data.emplace_back(it);
            }
            std::sort(ordered_data.begin(), ordered_data.end(),
                      [](const auto& lhs, const auto& rhs) { return lhs->first < rhs->first; });
            for (const auto& it : ordered_data) {
                append_kv(it->first, it->second);
            }
        }
        else {
            for (const auto& [key, val] : _object_data) {
                append_kv(key, val);
            }
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

    template<typename StringType>
    MEOJSON_INLINE bool basic_object<StringType>::get(const StringType& key, bool default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_boolean()) {
                return basic_value.as_boolean();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE int basic_object<StringType>::get(const StringType& key, int default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_integer();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long basic_object<StringType>::get(const StringType& key, long default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE unsigned long basic_object<StringType>::get(const StringType& key, unsigned default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_unsigned_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long long basic_object<StringType>::get(const StringType& key, long long default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE unsigned long long basic_object<StringType>::get(const StringType& key, unsigned long long default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_unsigned_long_long();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE float basic_object<StringType>::get(const StringType& key, float default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_float();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE double basic_object<StringType>::get(const StringType& key, double default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE long double basic_object<StringType>::get(const StringType& key, long double default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_number()) {
                return basic_value.as_long_double();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_object<StringType>::get(const StringType& key, StringType default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_string()) {
                return basic_value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const StringType basic_object<StringType>::get(const StringType& key, const char_t* default_value) const
    {
        if (contains(key)) {
            basic_value<StringType> basic_value = _object_data.at(key);
            if (basic_value.is_string()) {
                return basic_value.as_string();
            }
            else {
                return default_value;
            }
        }
        else {
            return default_value;
        }
    }

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType>& basic_object<StringType>::get(const StringType& key) const
    {
        if (contains(key)) {
            return _object_data.at(key);
        }
        else {
            static basic_value<StringType> null;
            return null;
        }
    }

    template<typename StringType>
    template <typename Type>
    MEOJSON_INLINE std::optional<Type> basic_object<StringType>::find(const StringType& key) const
    {
        static_assert(std::is_constructible_v<Type, basic_value<StringType>>, "Type can NOT be constructed by basic_value");
        auto iter = _object_data.find(key);
        if (iter == _object_data.end()) {
            return std::nullopt;
        }
        const auto& val = iter->second;
        return val.is<Type>() ? std::optional<Type>(val.as<Type>()) : std::nullopt;
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_object<StringType>::iterator basic_object<StringType>::begin() noexcept
    {
        return _object_data.begin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_object<StringType>::iterator basic_object<StringType>::end() noexcept
    {
        return _object_data.end();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_object<StringType>::const_iterator basic_object<StringType>::begin() const noexcept
    {
        return _object_data.begin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_object<StringType>::const_iterator basic_object<StringType>::end() const noexcept
    {
        return _object_data.end();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_object<StringType>::const_iterator basic_object<StringType>::cbegin() const noexcept
    {
        return _object_data.cbegin();
    }

    template<typename StringType>
    MEOJSON_INLINE typename basic_object<StringType>::const_iterator basic_object<StringType>::cend() const noexcept
    {
        return _object_data.cend();
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_object<StringType>::operator[](const StringType& key)
    {
        return _object_data[key];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_value<StringType>& basic_object<StringType>::operator[](StringType&& key)
    {
        return _object_data[std::move(key)];
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType> basic_object<StringType>::operator|(const basic_object<StringType>& rhs)&
    {
        basic_object<StringType> temp = *this;
        temp._object_data.insert(rhs.begin(), rhs.end());
        return temp;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType> basic_object<StringType>::operator|(basic_object<StringType>&& rhs)&
    {
        basic_object<StringType> temp = *this;
        // temp._object_data.merge(std::move(rhs._object_data));
        temp._object_data.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
        return temp;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType> basic_object<StringType>::operator|(const basic_object<StringType>& rhs)&&
    {
        _object_data.insert(rhs.begin(), rhs.end());
        return std::move(*this);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType> basic_object<StringType>::operator|(basic_object<StringType>&& rhs)&&
    {
        //_object_data.merge(std::move(rhs._object_data));
        _object_data.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
        return std::move(*this);
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>& basic_object<StringType>::operator|=(const basic_object<StringType>& rhs)
    {
        _object_data.insert(rhs.begin(), rhs.end());
        return *this;
    }

    template<typename StringType>
    MEOJSON_INLINE basic_object<StringType>& basic_object<StringType>::operator|=(basic_object<StringType>&& rhs)
    {
        _object_data.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
        return *this;
    }


    template<typename StringType>
    MEOJSON_INLINE bool basic_object<StringType>::operator==(const basic_object<StringType>& rhs) const
    {
        return _object_data == rhs._object_data;
    }

    // const raw_object &basic_object<StringType>::raw_data() const
    // {
    //     return _object_data;
    // }

    template<typename StringType>
    MEOJSON_INLINE std::ostream& operator<<(std::ostream& out, const basic_object<StringType>& obj)
    {
        // TODO: format output

        out << obj.to_string();
        return out;
    }

    template<typename StringType>
    template <typename MapType, typename EnableT>
    basic_object<StringType>::basic_object(MapType map)
    {
        _object_data.insert(std::make_move_iterator(map.begin()), std::make_move_iterator(map.end()));
    }

    // *************************
    // *     parser declare    *
    // *************************
    template <typename ParserType, typename StringType = default_StringType>
    class parser
    {
    public:
        using StringIterT = typename ParserType::const_iterator;

    public:
        ~parser() noexcept = default;

        static std::optional<typename basic_value<StringType>> parse(const ParserType& content);

    private:
        parser(StringIterT cbegin, StringIterT cend) noexcept
            : _cur(cbegin), _end(cend)
        {
            ;
        }

        std::optional<typename basic_value<StringType>> parse();
        basic_value<StringType> parse_value();

        basic_value<StringType> parse_null();
        basic_value<StringType> parse_boolean();
        basic_value<StringType> parse_number();
        // parse and return a basic_value<StringType> whose type is value_type::String
        basic_value<StringType> parse_string();
        basic_value<StringType> parse_array();
        basic_value<StringType> parse_object();

        // parse and return a StringType
        std::optional<StringType> parse_stdstring();

        bool skip_whitespace() noexcept;
        bool skip_digit();

    private:
        StringIterT _cur;
        StringIterT _end;
    };

    // *************************
    // *      utils impl       *
    // *************************

    template<typename StringType>
    MEOJSON_INLINE const basic_value<StringType> invalid_value()
    {
        return basic_value<StringType>(basic_value<StringType>::value_type::Invalid, basic_value<StringType>::var_t());
    }

    template <typename ParserType, typename StringType = default_StringType>
    MEOJSON_INLINE std::optional<typename basic_value<StringType>> parse(const ParserType& content)
    {
        return parser<ParserType, StringType>::parse(content);
    }

    template <typename StringType>
    MEOJSON_INLINE std::ostream& operator<<(std::ostream& out, const basic_value<StringType>& val)
    {
        // TODO: format output

        out << val.to_string();
        return out;
    }

    // TODO
    // std::istream &operator>>(std::istream &in, basic_value &val)
    //{
    //    return in;
    //}

    template <typename StringType>
    MEOJSON_INLINE std::optional<typename basic_value<StringType>> open(std::ifstream& ifs, bool check_bom = false)
    {
        if (!ifs.is_open()) {
            return std::nullopt;
        }

        ifs.seekg(0, std::ios::end);
        auto file_size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        StringType str(file_size, '\0');
        ifs.read(str.data(), file_size);

        if (check_bom) {
            using uchar = unsigned char;
            static constexpr uchar Bom_0 = 0xEF;
            static constexpr uchar Bom_1 = 0xBB;
            static constexpr uchar Bom_2 = 0xBF;

            if (str.size() >= 3 && static_cast<uchar>(str.at(0)) == Bom_0 && static_cast<uchar>(str.at(1)) == Bom_1 &&
                static_cast<uchar>(str.at(2)) == Bom_2) {
                str.assign(str.begin() + 3, str.end());
            }
        }

        return parse(str);
    }

    template <typename InputFilename, typename StringType = default_StringType>
    MEOJSON_INLINE std::optional<typename basic_value<StringType>> open(const InputFilename& filepath, bool check_bom = false)
    {
        static_assert(std::is_constructible_v<std::ifstream, InputFilename>,
                      "InputFilename can't be used to construct a std::ifstream");

        std::ifstream ifs(filepath, std::ios::in);
        auto opt = open<StringType>(ifs, check_bom);
        ifs.close();
        return opt;
    }

    // *************************
    // *      parser impl      *
    // *************************

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE std::optional<typename basic_value<StringType>> parser<ParserType, StringType>::parse(const ParserType& content)
    {
        return parser<ParserType, StringType>(content.cbegin(), content.cend()).parse();
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE std::optional<typename basic_value<StringType>> parser<ParserType, StringType>::parse()
    {
        if (!skip_whitespace()) {
            return std::nullopt;
        }

        basic_value<StringType> result_value;
        switch (*_cur) {
        case '[':
            result_value = parse_array();
            break;
        case '{':
            result_value = parse_object();
            break;
        default: // A JSON payload should be an basic_object or basic_array
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

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_value()
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

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_null()
    {
        static constexpr std::string_view null_string = "null";

        for (const char& ch : null_string) {
            if (*_cur == ch) {
                ++_cur;
            }
            else {
                return invalid_value();
            }
        }

        return basic_value<StringType>();
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_boolean()
    {
        static constexpr std::string_view true_string = "true";
        static constexpr std::string_view false_string = "false";

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

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_number()
    {
        const auto first = _cur;
        if (*_cur == '-') {
            ++_cur;
        }

        // Numbers cannot have leading zeroes
        if (_cur != _end && *_cur == '0' && _cur + 1 != _end && std::isdigit(*(_cur + 1))) {
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

        return basic_value<StringType>(basic_value<StringType>::value_type::Number, StringType(first, _cur));
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_string()
    {
        auto string_opt = parse_stdstring();
        if (!string_opt) {
            return invalid_value();
        }
        return basic_value<StringType>(basic_value<StringType>::value_type::String, std::move(string_opt).value());
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_array()
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
            // empty basic_array
            return basic_array<StringType>();
        }

        basic_array<StringType>::raw_array result;
        result.reserve(4);
        while (true) {
            if (!skip_whitespace()) {
                return invalid_value();
            }

            basic_value<StringType> val = parse_value();

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

        return basic_array<StringType>(std::move(result));
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE basic_value<StringType> parser<ParserType, StringType>::parse_object()
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
            // empty basic_object
            return basic_object<StringType>();
        }

        basic_object<StringType>::raw_object result;
        result.reserve(4);
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

            basic_value<StringType> val = parse_value();

            if (!val.valid() || !skip_whitespace()) {
                return invalid_value();
            }

            StringType key_escape = escape_string(std::move(key_opt).value());
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

        return basic_object<StringType>(std::move(result));
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE std::optional<StringType> parser<ParserType, StringType>::parse_stdstring()
    {
        if (*_cur == '"') {
            ++_cur;
        }
        else {
            return std::nullopt;
        }

        const auto first = _cur;
        auto last = _cur;
        bool is_string_end = false;
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

        return StringType(first, last);
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE bool parser<ParserType, StringType>::skip_whitespace() noexcept
    {
        while (_cur != _end) {
            switch (*_cur) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                ++_cur;
                break;
            case '\0':
                return false;
            default:
                return true;
            }
        }
        return false;
    }

    template<typename ParserType, typename StringType>
    MEOJSON_INLINE bool parser<ParserType, StringType>::skip_digit()
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

    // *************************
    // *      aux impl         *
    // *************************

    template<typename StringType>
    MEOJSON_INLINE StringType unescape_string(StringType str)
    {
        for (size_t pos = 0; pos < str.size(); ++pos) {
            StringType replace_str;
            switch (str[pos]) {
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
            str.replace(pos, 1, replace_str);
            ++pos;
        }
        return str;
    }

    template<typename StringType>
    MEOJSON_INLINE StringType escape_string(StringType str)
    {
        for (size_t pos = 0; pos + 1 < str.size(); ++pos) {
            if (str[pos] != '\\') {
                continue;
            }
            StringType replace_str;
            switch (str[pos + 1]) {
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
                return StringType();
                break;
            }
            str.replace(pos, 2, replace_str);
        }
        return str;
    }
} // namespace json
