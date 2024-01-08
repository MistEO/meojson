#pragma once

#include <fstream>
#include <initializer_list>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "packed_bytes.hpp"

#if __cplusplus >= 202002L || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L)
#define __json_enable_constexpr
#include "constexpr_helper.hpp"

#define __json_constexpr constexpr
#else
#define __json_constexpr
#endif

#define __json_template        \
    typename string_t,         \
        template <typename...> \
        typename map_t,        \
        template <typename...> \
        typename unique_ptr_t, \
        template <typename...> \
        typename variant_t
#define __json_template_default              \
    typename string_t = default_string_t,    \
             template <typename...>          \
             typename map_t = default_map_t, \
             template <typename...>          \
             typename unique_ptr_t = default_unique_ptr_t, \
             template <typename...>          \
             typename variant_t = default_variant_t
#define __json_template_arg string_t, map_t, unique_ptr_t, variant_t

namespace json
{

template <__json_template>
class basic_value;
template <__json_template>
class basic_array;
template <__json_template>
class basic_object;

using default_string_t = std::string;
template <typename... _>
using default_map_t = std::map<_...>;
template <typename... _>
using default_unique_ptr_t = std::unique_ptr<_...>;
template <typename... _>
using default_variant_t = std::variant<_...>;

using value = basic_value<default_string_t, default_map_t, default_unique_ptr_t, default_variant_t>;
using array = basic_array<default_string_t, default_map_t, default_unique_ptr_t, default_variant_t>;
using object = basic_object<default_string_t, default_map_t, default_unique_ptr_t, default_variant_t>;

using wvalue = basic_value<std::wstring, default_map_t, default_unique_ptr_t, default_variant_t>;
using warray = basic_array<std::wstring, default_map_t, default_unique_ptr_t, default_variant_t>;
using wobject = basic_object<std::wstring, default_map_t, default_unique_ptr_t, default_variant_t>;

#ifdef __json_enable_constexpr

using cvalue = basic_value<default_string_t, constexpr_map, constexpr_unique_ptr, constexpr_variant>;
using carray = basic_array<default_string_t, constexpr_map, constexpr_unique_ptr, constexpr_variant>;
using cobject = basic_object<default_string_t, constexpr_map, constexpr_unique_ptr, constexpr_variant>;

#endif

namespace utils
{
    template <typename T>
    using iterator_t = decltype(std::declval<T&>().begin());
    template <typename T>
    using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
    template <typename T>
    using iter_value_t = typename std::iterator_traits<remove_cvref_t<T>>::value_type;
    template <typename R>
    using range_value_t = iter_value_t<iterator_t<R>>;
}

// *********************************
// *      basic_value declare      *
// *********************************

template <__json_template>
class basic_value
{
    using array_ptr = unique_ptr_t<basic_array<__json_template_arg>>;
    using object_ptr = unique_ptr_t<basic_object<__json_template_arg>>;

public:
    enum class value_type : char
    {
        invalid,
        null,
        boolean,
        string,
        number,
        array,
        object
    };

    using var_t = variant_t<string_t, array_ptr, object_ptr>;
    using char_t = typename string_t::value_type;

public:
    __json_constexpr basic_value();
    __json_constexpr basic_value(const basic_value<__json_template_arg>& rhs);
    __json_constexpr basic_value(basic_value<__json_template_arg>&& rhs) noexcept;

    __json_constexpr basic_value(bool b);

    __json_constexpr basic_value(int num);
    __json_constexpr basic_value(unsigned num);
    __json_constexpr basic_value(long num);
    __json_constexpr basic_value(unsigned long num);
    __json_constexpr basic_value(long long num);
    __json_constexpr basic_value(unsigned long long num);
    __json_constexpr basic_value(float num);
    __json_constexpr basic_value(double num);
    __json_constexpr basic_value(long double num);

    __json_constexpr basic_value(const char_t* str);
    __json_constexpr basic_value(string_t str);

    __json_constexpr basic_value(basic_array<__json_template_arg> arr);
    __json_constexpr basic_value(basic_object<__json_template_arg> obj);
    __json_constexpr basic_value(std::initializer_list<basic_value<__json_template_arg>> init_list);

    // Constructed from raw data
    template <typename... args_t>
    __json_constexpr basic_value(value_type type, args_t&&... args);

    template <typename collection_t,
              std::enable_if_t<std::is_constructible_v<typename basic_array<__json_template_arg>::value_type,
                                                       utils::range_value_t<collection_t>>,
                               bool> = true>
    __json_constexpr basic_value(collection_t&& collection)
        : basic_value(basic_array<__json_template_arg>(std::forward<collection_t>(collection)))
    {}
    template <typename another_map_t,
              std::enable_if_t<std::is_constructible_v<typename basic_object<__json_template_arg>::value_type,
                                                       utils::range_value_t<another_map_t>>,
                               bool> = true>
    __json_constexpr basic_value(another_map_t&& map)
        : basic_value(basic_object<__json_template_arg>(std::forward<another_map_t>(map)))
    {}

    template <typename value_t,
              std::enable_if_t<!std::is_convertible_v<value_t, basic_value<__json_template_arg>>, bool> = true>
    basic_value(value_t) = delete;

    // I don't know if you want to convert char to string or number, so I delete these constructors.
    basic_value(char) = delete;
    basic_value(wchar_t) = delete;
    basic_value(char16_t) = delete;
    basic_value(char32_t) = delete;

    __json_constexpr ~basic_value();

    __json_constexpr bool valid() const noexcept { return _type != value_type::invalid; }
    __json_constexpr bool empty() const noexcept { return is_null(); }
    __json_constexpr bool is_null() const noexcept { return _type == value_type::null; }
    __json_constexpr bool is_number() const noexcept { return _type == value_type::number; }
    __json_constexpr bool is_boolean() const noexcept { return _type == value_type::boolean; }
    __json_constexpr bool is_string() const noexcept { return _type == value_type::string; }
    __json_constexpr bool is_array() const noexcept { return _type == value_type::array; }
    __json_constexpr bool is_object() const noexcept { return _type == value_type::object; }
    template <typename value_t>
    __json_constexpr bool is() const noexcept;

    __json_constexpr bool contains(const string_t& key) const;
    __json_constexpr bool contains(size_t pos) const;
    __json_constexpr bool exists(const string_t& key) const { return contains(key); }
    __json_constexpr bool exists(size_t pos) const { return contains(pos); }
    __json_constexpr value_type type() const noexcept { return _type; }
    __json_constexpr const basic_value<__json_template_arg>& at(size_t pos) const;
    __json_constexpr const basic_value<__json_template_arg>& at(const string_t& key) const;

    __json_constexpr bool erase(size_t pos);
    __json_constexpr bool erase(const string_t& key);

    // Usage: get(key_1, key_2, ..., default_value);
    template <typename... key_then_default_value_t>
    __json_constexpr auto get(key_then_default_value_t&&... keys_then_default_value) const;

    template <typename value_t = basic_value<__json_template_arg>>
    __json_constexpr std::optional<value_t> find(size_t pos) const;
    template <typename value_t = basic_value<__json_template_arg>>
    __json_constexpr std::optional<value_t> find(const string_t& key) const;

    __json_constexpr bool as_boolean() const;
    __json_constexpr int as_integer() const;
    __json_constexpr unsigned as_unsigned() const;
    __json_constexpr long as_long() const;
    __json_constexpr unsigned long as_unsigned_long() const;
    __json_constexpr long long as_long_long() const;
    __json_constexpr unsigned long long as_unsigned_long_long() const;
    __json_constexpr float as_float() const;
    __json_constexpr double as_double() const;
    __json_constexpr long double as_long_double() const;
    __json_constexpr string_t as_string() const;
    __json_constexpr const basic_array<__json_template_arg>& as_array() const;
    __json_constexpr const basic_object<__json_template_arg>& as_object() const;
    template <typename value_t>
    __json_constexpr value_t as() const;

    __json_constexpr basic_array<__json_template_arg>& as_array();
    __json_constexpr basic_object<__json_template_arg>& as_object();

    template <typename... args_t>
    __json_constexpr decltype(auto) emplace(args_t&&... args);

    template <typename... args_t>
    /*will be deprecated, please use `emplace` instead.*/
    __json_constexpr decltype(auto) array_emplace(args_t&&... args);
    template <typename... args_t>
    /*will be deprecated, please use `emplace` instead.*/
    __json_constexpr decltype(auto) object_emplace(args_t&&... args);

    __json_constexpr void clear() noexcept;

    __json_constexpr string_t dumps(std::optional<size_t> indent = std::nullopt) const
    {
        return indent ? format(*indent) : to_string();
    }
    // return raw string
    __json_constexpr string_t to_string() const;
    __json_constexpr string_t format() const { return format(4, 0); }
    // format(bool) is deprecated now.
    template <typename sz_t, typename = std::enable_if_t<std::is_integral_v<sz_t> && !std::is_same_v<sz_t, bool>>>
    __json_constexpr string_t format(sz_t indent) const
    {
        return format(indent, 0);
    }

    template <typename value_t>
    __json_constexpr bool all() const;
    template <typename value_t, template <typename...> typename vector_t = std::vector>
    __json_constexpr vector_t<value_t> to_vector() const;
    template <typename value_t, template <typename...> typename another_map_t = std::map>
    __json_constexpr another_map_t<string_t, value_t> to_map() const;

    __json_constexpr basic_value<__json_template_arg>& operator=(const basic_value<__json_template_arg>& rhs);
    __json_constexpr basic_value<__json_template_arg>& operator=(basic_value<__json_template_arg>&&) noexcept;

    __json_constexpr bool operator==(const basic_value<__json_template_arg>& rhs) const;
    __json_constexpr bool operator!=(const basic_value<__json_template_arg>& rhs) const { return !(*this == rhs); }

    __json_constexpr const basic_value<__json_template_arg>& operator[](size_t pos) const;
    __json_constexpr basic_value<__json_template_arg>& operator[](size_t pos);
    __json_constexpr basic_value<__json_template_arg>& operator[](const string_t& key);
    __json_constexpr basic_value<__json_template_arg>& operator[](string_t&& key);

    __json_constexpr basic_value<__json_template_arg> operator|(const basic_object<__json_template_arg>& rhs) const&;
    __json_constexpr basic_value<__json_template_arg> operator|(basic_object<__json_template_arg>&& rhs) const&;
    __json_constexpr basic_value<__json_template_arg> operator|(const basic_object<__json_template_arg>& rhs) &&;
    __json_constexpr basic_value<__json_template_arg> operator|(basic_object<__json_template_arg>&& rhs) &&;

    __json_constexpr basic_value<__json_template_arg>& operator|=(const basic_object<__json_template_arg>& rhs);
    __json_constexpr basic_value<__json_template_arg>& operator|=(basic_object<__json_template_arg>&& rhs);

    __json_constexpr basic_value<__json_template_arg> operator+(const basic_array<__json_template_arg>& rhs) const&;
    __json_constexpr basic_value<__json_template_arg> operator+(basic_array<__json_template_arg>&& rhs) const&;
    __json_constexpr basic_value<__json_template_arg> operator+(const basic_array<__json_template_arg>& rhs) &&;
    __json_constexpr basic_value<__json_template_arg> operator+(basic_array<__json_template_arg>&& rhs) &&;

    __json_constexpr basic_value<__json_template_arg>& operator+=(const basic_array<__json_template_arg>& rhs);
    __json_constexpr basic_value<__json_template_arg>& operator+=(basic_array<__json_template_arg>&& rhs);

    __json_constexpr explicit operator bool() const { return as_boolean(); }
    __json_constexpr explicit operator int() const { return as_integer(); }
    __json_constexpr explicit operator unsigned() const { return as_unsigned(); }
    __json_constexpr explicit operator long() const { return as_long(); }
    __json_constexpr explicit operator unsigned long() const { return as_unsigned_long(); }
    __json_constexpr explicit operator long long() const { return as_long_long(); }
    __json_constexpr explicit operator unsigned long long() const { return as_unsigned_long_long(); }
    __json_constexpr explicit operator float() const { return as_float(); }
    __json_constexpr explicit operator double() const { return as_double(); }
    __json_constexpr explicit operator long double() const { return as_long_double(); }
    __json_constexpr explicit operator string_t() const { return as_string(); }

private:
    friend class basic_array<__json_template_arg>;
    friend class basic_object<__json_template_arg>;

    __json_constexpr string_t format(size_t indent, size_t indent_times) const;

    __json_constexpr static var_t deep_copy(const var_t& src);

    template <typename... key_then_default_value_t, size_t... keys_indexes_t>
    __json_constexpr auto get(std::tuple<key_then_default_value_t...> keys_then_default_value,
                              std::index_sequence<keys_indexes_t...>) const;

    template <typename value_t, typename first_key_t, typename... rest_keys_t>
    __json_constexpr auto get_helper(const value_t& default_value, first_key_t&& first, rest_keys_t&&... rest) const;
    template <typename value_t, typename unique_key_t>
    __json_constexpr auto get_helper(const value_t& default_value, unique_key_t&& first) const;

    __json_constexpr const string_t& as_basic_type_str() const;
    __json_constexpr string_t& as_basic_type_str();

    value_type _type = value_type::null;
    var_t _raw_data;
};

// *********************************
// *      basic_array declare      *
// *********************************

template <__json_template>
class basic_array
{
    friend class basic_value<__json_template_arg>;
    friend class basic_object<__json_template_arg>;

public:
    using raw_array = std::vector<basic_value<__json_template_arg>>;
    using value_type = typename raw_array::value_type;
    using iterator = typename raw_array::iterator;
    using const_iterator = typename raw_array::const_iterator;
    using reverse_iterator = typename raw_array::reverse_iterator;
    using const_reverse_iterator = typename raw_array::const_reverse_iterator;
    using char_t = typename string_t::value_type;

public:
    __json_constexpr basic_array() = default;
    __json_constexpr basic_array(const basic_array<__json_template_arg>& rhs) = default;
    __json_constexpr basic_array(basic_array<__json_template_arg>&& rhs) noexcept = default;
    __json_constexpr basic_array(std::initializer_list<value_type> init_list);
    __json_constexpr basic_array(typename raw_array::size_type size);

    __json_constexpr explicit basic_array(const basic_value<__json_template_arg>& val);
    __json_constexpr explicit basic_array(basic_value<__json_template_arg>&& val);

    template <typename collection_t,
              typename = std::enable_if_t<std::is_constructible_v<value_type, utils::range_value_t<collection_t>>>>
    __json_constexpr basic_array(collection_t arr)
        : _array_data(std::make_move_iterator(arr.begin()), std::make_move_iterator(arr.end()))
    {}

    __json_constexpr ~basic_array() noexcept = default;

    __json_constexpr bool empty() const noexcept { return _array_data.empty(); }
    __json_constexpr size_t size() const noexcept { return _array_data.size(); }
    __json_constexpr bool contains(size_t pos) const { return pos < _array_data.size(); }
    __json_constexpr bool exists(size_t pos) const { return contains(pos); }
    __json_constexpr const basic_value<__json_template_arg>& at(size_t pos) const;

    __json_constexpr string_t dumps(std::optional<size_t> indent = std::nullopt) const
    {
        return indent ? format(*indent) : to_string();
    }
    __json_constexpr string_t to_string() const;
    __json_constexpr string_t format() const { return format(4, 0); }
    template <typename sz_t, typename = std::enable_if_t<std::is_integral_v<sz_t> && !std::is_same_v<sz_t, bool>>>
    __json_constexpr string_t format(sz_t indent) const
    {
        return format(indent, 0);
    }
    template <typename value_t>
    __json_constexpr bool all() const;
    template <typename value_t, template <typename...> typename vector_t = std::vector>
    __json_constexpr vector_t<value_t> to_vector() const;

    // Usage: get(key_1, key_2, ..., default_value);
    template <typename... key_then_default_value_t>
    __json_constexpr auto get(key_then_default_value_t&&... keys_then_default_value) const;

    template <typename value_t = basic_value<__json_template_arg>>
    __json_constexpr std::optional<value_t> find(size_t pos) const;

    template <typename... args_t>
    __json_constexpr decltype(auto) emplace_back(args_t&&... args);
    template <typename... args_t>
    __json_constexpr decltype(auto) push_back(args_t&&... args);

    __json_constexpr void clear() noexcept;
    __json_constexpr bool erase(size_t pos);
    __json_constexpr bool erase(iterator iter);

    __json_constexpr iterator begin() noexcept;
    __json_constexpr iterator end() noexcept;
    __json_constexpr const_iterator begin() const noexcept;
    __json_constexpr const_iterator end() const noexcept;
    __json_constexpr const_iterator cbegin() const noexcept;
    __json_constexpr const_iterator cend() const noexcept;

    __json_constexpr reverse_iterator rbegin() noexcept;
    __json_constexpr reverse_iterator rend() noexcept;
    __json_constexpr const_reverse_iterator rbegin() const noexcept;
    __json_constexpr const_reverse_iterator rend() const noexcept;
    __json_constexpr const_reverse_iterator crbegin() const noexcept;
    __json_constexpr const_reverse_iterator crend() const noexcept;

    __json_constexpr const basic_value<__json_template_arg>& operator[](size_t pos) const;
    __json_constexpr basic_value<__json_template_arg>& operator[](size_t pos);

    __json_constexpr basic_array<__json_template_arg> operator+(const basic_array<__json_template_arg>& rhs) const&;
    __json_constexpr basic_array<__json_template_arg> operator+(basic_array<__json_template_arg>&& rhs) const&;
    __json_constexpr basic_array<__json_template_arg> operator+(const basic_array<__json_template_arg>& rhs) &&;
    __json_constexpr basic_array<__json_template_arg> operator+(basic_array<__json_template_arg>&& rhs) &&;

    __json_constexpr basic_array<__json_template_arg>& operator+=(const basic_array<__json_template_arg>& rhs);
    __json_constexpr basic_array<__json_template_arg>& operator+=(basic_array<__json_template_arg>&& rhs);

    __json_constexpr basic_array<__json_template_arg>& operator=(const basic_array<__json_template_arg>&) = default;
    __json_constexpr basic_array<__json_template_arg>& operator=(basic_array<__json_template_arg>&&) noexcept = default;

    __json_constexpr bool operator==(const basic_array<__json_template_arg>& rhs) const;
    __json_constexpr bool operator!=(const basic_array<__json_template_arg>& rhs) const { return !(*this == rhs); }

private:
    template <typename... key_then_default_value_t, size_t... keys_indexes_t>
    __json_constexpr auto get(std::tuple<key_then_default_value_t...> keys_then_default_value,
                              std::index_sequence<keys_indexes_t...>) const;
    template <typename value_t, typename... rest_keys_t>
    __json_constexpr auto get_helper(const value_t& default_value, size_t pos, rest_keys_t&&... rest) const;
    template <typename value_t>
    __json_constexpr auto get_helper(const value_t& default_value, size_t pos) const;

    __json_constexpr string_t format(size_t indent, size_t indent_times) const;

private:
    raw_array _array_data;
};

// **********************************
// *      basic_object declare      *
// **********************************

template <__json_template>
class basic_object
{
    friend class basic_value<__json_template_arg>;
    friend class basic_array<__json_template_arg>;

public:
    using raw_object = map_t<string_t, basic_value<__json_template_arg>>;
    using key_type = typename raw_object::key_type;
    using mapped_type = typename raw_object::mapped_type;
    using value_type = typename raw_object::value_type;
    using iterator = typename raw_object::iterator;
    using const_iterator = typename raw_object::const_iterator;
    using char_t = typename string_t::value_type;

public:
    __json_constexpr basic_object() = default;
    __json_constexpr basic_object(const basic_object<__json_template_arg>& rhs) = default;
    __json_constexpr basic_object(basic_object<__json_template_arg>&& rhs) noexcept = default;
    __json_constexpr basic_object(std::initializer_list<value_type> init_list);
    __json_constexpr explicit basic_object(const basic_value<__json_template_arg>& val);
    __json_constexpr explicit basic_object(basic_value<__json_template_arg>&& val);
    template <typename another_map_t,
              typename = std::enable_if_t<std::is_constructible_v<value_type, utils::range_value_t<another_map_t>>>>
    __json_constexpr basic_object(another_map_t map)
        : _object_data(std::make_move_iterator(map.begin()), std::make_move_iterator(map.end()))
    {}

    __json_constexpr ~basic_object() = default;

    __json_constexpr bool empty() const noexcept { return _object_data.empty(); }
    __json_constexpr size_t size() const noexcept { return _object_data.size(); }
    __json_constexpr bool contains(const string_t& key) const;
    __json_constexpr bool exists(const string_t& key) const { return contains(key); }
    __json_constexpr const basic_value<__json_template_arg>& at(const string_t& key) const;

    __json_constexpr string_t dumps(std::optional<size_t> indent = std::nullopt) const
    {
        return indent ? format(*indent) : to_string();
    }
    __json_constexpr string_t to_string() const;
    __json_constexpr string_t format() const { return format(4, 0); }
    template <typename sz_t, typename = std::enable_if_t<std::is_integral_v<sz_t> && !std::is_same_v<sz_t, bool>>>
    __json_constexpr string_t format(sz_t indent) const
    {
        return format(indent, 0);
    }
    template <typename value_t>
    __json_constexpr bool all() const;
    template <typename value_t, template <typename...> typename another_map_t = default_map_t>
    __json_constexpr another_map_t<string_t, value_t> to_map() const;

    // Usage: get(key_1, key_2, ..., default_value);
    template <typename... key_then_default_value_t>
    __json_constexpr auto get(key_then_default_value_t&&... keys_then_default_value) const;

    template <typename value_t = basic_value<__json_template_arg>>
    __json_constexpr std::optional<value_t> find(const string_t& key) const;

    template <typename... args_t>
    __json_constexpr decltype(auto) emplace(args_t&&... args);
    template <typename... args_t>
    __json_constexpr decltype(auto) insert(args_t&&... args);

    __json_constexpr void clear() noexcept;
    __json_constexpr bool erase(const string_t& key);
    __json_constexpr bool erase(iterator iter);

    __json_constexpr iterator begin() noexcept;
    __json_constexpr iterator end() noexcept;
    __json_constexpr const_iterator begin() const noexcept;
    __json_constexpr const_iterator end() const noexcept;
    __json_constexpr const_iterator cbegin() const noexcept;
    __json_constexpr const_iterator cend() const noexcept;

    __json_constexpr basic_value<__json_template_arg>& operator[](const string_t& key);
    __json_constexpr basic_value<__json_template_arg>& operator[](string_t&& key);

    __json_constexpr basic_object<__json_template_arg> operator|(const basic_object<__json_template_arg>& rhs) const&;
    __json_constexpr basic_object<__json_template_arg> operator|(basic_object<__json_template_arg>&& rhs) const&;
    __json_constexpr basic_object<__json_template_arg> operator|(const basic_object<__json_template_arg>& rhs) &&;
    __json_constexpr basic_object<__json_template_arg> operator|(basic_object<__json_template_arg>&& rhs) &&;

    __json_constexpr basic_object<__json_template_arg>& operator|=(const basic_object<__json_template_arg>& rhs);
    __json_constexpr basic_object<__json_template_arg>& operator|=(basic_object<__json_template_arg>&& rhs);

    __json_constexpr basic_object<__json_template_arg>& operator=(const basic_object<__json_template_arg>&) = default;
    __json_constexpr basic_object<__json_template_arg>& operator=(basic_object<__json_template_arg>&&) = default;

    __json_constexpr bool operator==(const basic_object<__json_template_arg>& rhs) const;
    __json_constexpr bool operator!=(const basic_object<__json_template_arg>& rhs) const { return !(*this == rhs); }

private:
    template <typename... key_then_default_value_t, size_t... keys_indexes_t>
    __json_constexpr auto get(std::tuple<key_then_default_value_t...> keys_then_default_value,
                              std::index_sequence<keys_indexes_t...>) const;
    template <typename value_t, typename... rest_keys_t>
    __json_constexpr auto get_helper(const value_t& default_value, const string_t& key, rest_keys_t&&... rest) const;
    template <typename value_t>
    __json_constexpr auto get_helper(const value_t& default_value, const string_t& key) const;

    __json_constexpr string_t format(size_t indent, size_t indent_times) const;

private:
    raw_object _object_data;
};

// ****************************
// *      parser declare      *
// ****************************

template <__json_template_default, typename parsing_t = void,
          typename accel_traits = _packed_bytes::packed_bytes_trait_max>
class parser
{
public:
    using parsing_iter_t = typename parsing_t::const_iterator;

public:
    __json_constexpr ~parser() noexcept = default;

    __json_constexpr static std::optional<basic_value<__json_template_arg>> parse(const parsing_t& content);

private:
    __json_constexpr parser(parsing_iter_t cbegin, parsing_iter_t cend) noexcept : _cur(cbegin), _end(cend) { ; }

    __json_constexpr std::optional<basic_value<__json_template_arg>> parse();
    __json_constexpr basic_value<__json_template_arg> parse_value();

    __json_constexpr basic_value<__json_template_arg> parse_null();
    __json_constexpr basic_value<__json_template_arg> parse_boolean();
    __json_constexpr basic_value<__json_template_arg> parse_number();
    // parse and return a basic_value<__json_template_arg> whose type is value_type::string
    __json_constexpr basic_value<__json_template_arg> parse_string();
    __json_constexpr basic_value<__json_template_arg> parse_array();
    __json_constexpr basic_value<__json_template_arg> parse_object();

    // parse and return a string_t
    __json_constexpr std::optional<string_t> parse_stdstring();

    __json_constexpr bool skip_string_literal_with_accel();
    __json_constexpr bool skip_whitespace() noexcept;
    __json_constexpr bool skip_digit();

private:
    parsing_iter_t _cur;
    parsing_iter_t _end;
};

// *******************************
// *      exception declare      *
// *******************************

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

    virtual const char* what() const noexcept override { return _what.empty() ? "Unknown exception" : _what.c_str(); }

protected:
    std::string _what;
};

// ***************************
// *      utils declare      *
// ***************************

template <typename parsing_t>
__json_constexpr auto parse(const parsing_t& content);

#ifdef __json_enable_constexpr
template <typename parsing_t>
__json_constexpr auto cparse(const parsing_t& content);
#endif

template <typename char_t>
__json_constexpr auto parse(char_t* content);

template <typename istream_t,
          typename = std::enable_if_t<std::is_base_of_v<std::basic_istream<typename istream_t::char_type>, istream_t>>>
__json_constexpr auto parse(istream_t& istream, bool check_bom);

template <typename ifstream_t = std::ifstream, typename path_t = void>
auto open(const path_t& path, bool check_bom = false);

namespace literals
{
    value operator""_json(const char* str, size_t len);
    wvalue operator""_json(const wchar_t* str, size_t len);

    value operator""_jvalue(const char* str, size_t len);
    wvalue operator""_jvalue(const wchar_t* str, size_t len);

    array operator""_jarray(const char* str, size_t len);
    warray operator""_jarray(const wchar_t* str, size_t len);

    object operator""_jobject(const char* str, size_t len);
    wobject operator""_jobject(const wchar_t* str, size_t len);

#ifdef __json_enable_constexpr
    __json_constexpr cvalue operator""_cjson(const char* str, size_t len);
    __json_constexpr cvalue operator""_cjvalue(const char* str, size_t len);
    __json_constexpr carray operator""_cjarray(const char* str, size_t len);
    __json_constexpr cobject operator""_cjobject(const char* str, size_t len);
#endif
}

template <__json_template_default>
__json_constexpr const basic_value<__json_template_arg> invalid_value();

namespace _serialization_helper
{
    template <bool loose, typename string_t>
    struct string_converter;
}
template <bool loose, typename any_t, __json_template_default,
          typename string_converter_t = _serialization_helper::string_converter<loose, string_t>>
__json_constexpr basic_value<__json_template_arg> serialize(any_t&& arg, string_converter_t&& string_converter = {});

// ******************************
// *      basic_value impl      *
// ******************************

template <typename string_t>
static constexpr string_t true_string()
{
    return { 't', 'r', 'u', 'e' };
}

template <typename string_t>
static constexpr string_t false_string()
{
    return { 'f', 'a', 'l', 's', 'e' };
}

template <typename string_t>
static constexpr string_t null_string()
{
    return { 'n', 'u', 'l', 'l' };
}

template <typename string_t, typename any_t>
__json_constexpr string_t to_basic_string(any_t&& arg)
{
#ifdef __json_enable_constexpr
    if (std::is_constant_evaluated()) {
        return soft_to_string<string_t>(std::forward<any_t>(arg));
    }
#endif
    if constexpr (std::is_same_v<string_t, std::string>) {
        return std::to_string(std::forward<any_t>(arg));
    }
    else if constexpr (std::is_same_v<string_t, std::wstring>) {
        return std::to_wstring(std::forward<any_t>(arg));
    }
    else {
        static_assert(!sizeof(any_t), "Unsupported type");
    }
}

template <typename string_t>
static constexpr string_t unescape_string(const string_t& str);

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value() = default;

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(const basic_value<__json_template_arg>& rhs)
    : _type(rhs._type), _raw_data(deep_copy(rhs._raw_data))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(basic_value<__json_template_arg>&& rhs) noexcept =
    default;

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(bool b)
    : _type(value_type::boolean), _raw_data(string_t(b ? true_string<string_t>() : false_string<string_t>()))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(int num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(unsigned num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(long num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(unsigned long num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(long long num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(unsigned long long num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(float num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(double num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(long double num)
    : _type(value_type::number), _raw_data(to_basic_string<string_t>(num))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(const char_t* str)
    : _type(value_type::string), _raw_data(string_t(str))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(string_t str)
    : _type(value_type::string), _raw_data(std::move(str))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(basic_array<__json_template_arg> arr)
    : _type(value_type::array),
      _raw_data(unique_ptr_t<basic_array<__json_template_arg>>(new basic_array<__json_template_arg>(std::move(arr))))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(basic_object<__json_template_arg> obj)
    : _type(value_type::object),
      _raw_data(unique_ptr_t<basic_object<__json_template_arg>>(new basic_object<__json_template_arg>(std::move(obj))))
{}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(
    std::initializer_list<basic_value<__json_template_arg>> init_list)
    : _type(value_type::object),
      _raw_data(unique_ptr_t<basic_object<__json_template_arg>>(new basic_object<__json_template_arg>(init_list)))
{}

// for Pimpl
template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>::~basic_value() = default;

template <__json_template>
template <typename value_t>
inline __json_constexpr bool basic_value<__json_template_arg>::is() const noexcept
{
    if constexpr (std::is_same_v<basic_value<__json_template_arg>, value_t>) {
        return true;
    }
    else if constexpr (std::is_same_v<bool, value_t>) {
        return _type == value_type::boolean;
    }
    else if constexpr (std::is_arithmetic_v<value_t>) {
        return _type == value_type::number;
    }
    else if constexpr (std::is_same_v<basic_array<__json_template_arg>, value_t>) {
        return _type == value_type::array;
    }
    else if constexpr (std::is_same_v<basic_object<__json_template_arg>, value_t>) {
        return _type == value_type::object;
    }
    else if constexpr (std::is_constructible_v<string_t, value_t>) {
        return _type == value_type::string;
    }
    else {
        static_assert(!sizeof(value_t), "Unsupported type");
    }
}

template <__json_template>
inline __json_constexpr bool basic_value<__json_template_arg>::contains(const string_t& key) const
{
    return is_object() && as_object().contains(key);
}

template <__json_template>
inline __json_constexpr bool basic_value<__json_template_arg>::contains(size_t pos) const
{
    return is_array() && as_array().contains(pos);
}

template <__json_template>
inline __json_constexpr const basic_value<__json_template_arg>& basic_value<__json_template_arg>::at(size_t pos) const
{
    return as_array().at(pos);
}

template <__json_template>
inline __json_constexpr const basic_value<__json_template_arg>& basic_value<__json_template_arg>::at(
    const string_t& key) const
{
    return as_object().at(key);
}

template <__json_template>
inline __json_constexpr bool basic_value<__json_template_arg>::erase(size_t pos)
{
    return as_array().erase(pos);
}

template <__json_template>
inline __json_constexpr bool basic_value<__json_template_arg>::erase(const string_t& key)
{
    return as_object().erase(key);
}

template <__json_template>
template <typename... key_then_default_value_t>
inline __json_constexpr auto basic_value<__json_template_arg>::get(
    key_then_default_value_t&&... keys_then_default_value) const
{
    return get(std::forward_as_tuple(keys_then_default_value...),
               std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
}

template <__json_template>
template <typename... key_then_default_value_t, size_t... keys_indexes_t>
inline __json_constexpr auto basic_value<__json_template_arg>::get(
    std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const
{
    constexpr unsigned long default_value_index = sizeof...(key_then_default_value_t) - 1;
    return get_helper(std::get<default_value_index>(keys_then_default_value),
                      std::get<keys_indexes_t>(keys_then_default_value)...);
}

template <__json_template>
template <typename value_t, typename first_key_t, typename... rest_keys_t>
inline __json_constexpr auto basic_value<__json_template_arg>::get_helper(const value_t& default_value,
                                                                          first_key_t&& first,
                                                                          rest_keys_t&&... rest) const
{
    if constexpr (std::is_constructible_v<string_t, first_key_t>) {
        return is_object() ? as_object().get_helper(default_value, std::forward<first_key_t>(first),
                                                    std::forward<rest_keys_t>(rest)...)
                           : default_value;
    }
    else if constexpr (std::is_integral_v<std::decay_t<first_key_t>>) {
        return is_array() ? as_array().get_helper(default_value, std::forward<first_key_t>(first),
                                                  std::forward<rest_keys_t>(rest)...)
                          : default_value;
    }
    else {
        static_assert(!sizeof(first_key_t), "Parameter must be integral or string_t constructible");
    }
}

template <__json_template>
template <typename value_t, typename unique_key_t>
inline __json_constexpr auto basic_value<__json_template_arg>::get_helper(const value_t& default_value,
                                                                          unique_key_t&& first) const
{
    if constexpr (std::is_constructible_v<string_t, unique_key_t>) {
        return is_object() ? as_object().get_helper(default_value, std::forward<unique_key_t>(first)) : default_value;
    }
    else if constexpr (std::is_integral_v<std::decay_t<unique_key_t>>) {
        return is_array() ? as_array().get_helper(default_value, std::forward<unique_key_t>(first)) : default_value;
    }
    else {
        static_assert(!sizeof(unique_key_t), "Parameter must be integral or string_t constructible");
    }
}

template <__json_template>
template <typename... key_then_default_value_t>
inline __json_constexpr auto basic_array<__json_template_arg>::get(
    key_then_default_value_t&&... keys_then_default_value) const
{
    return get(std::forward_as_tuple(keys_then_default_value...),
               std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
}

template <__json_template>
template <typename... key_then_default_value_t, size_t... keys_indexes_t>
inline __json_constexpr auto basic_array<__json_template_arg>::get(
    std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const
{
    constexpr unsigned long default_value_index = sizeof...(key_then_default_value_t) - 1;
    return get_helper(std::get<default_value_index>(keys_then_default_value),
                      std::get<keys_indexes_t>(keys_then_default_value)...);
}

template <__json_template>
template <typename... key_then_default_value_t>
inline __json_constexpr auto basic_object<__json_template_arg>::get(
    key_then_default_value_t&&... keys_then_default_value) const
{
    return get(std::forward_as_tuple(keys_then_default_value...),
               std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
}

template <__json_template>
template <typename... key_then_default_value_t, size_t... keys_indexes_t>
inline __json_constexpr auto basic_object<__json_template_arg>::get(
    std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const
{
    constexpr unsigned long default_value_index = sizeof...(key_then_default_value_t) - 1;
    return get_helper(std::get<default_value_index>(keys_then_default_value),
                      std::get<keys_indexes_t>(keys_then_default_value)...);
}

template <__json_template>
template <typename value_t>
inline __json_constexpr std::optional<value_t> basic_value<__json_template_arg>::find(size_t pos) const
{
    return is_array() ? as_array().template find<value_t>(pos) : std::nullopt;
}

template <__json_template>
template <typename value_t>
inline __json_constexpr std::optional<value_t> basic_value<__json_template_arg>::find(const string_t& key) const
{
    return is_object() ? as_object().template find<value_t>(key) : std::nullopt;
}

template <__json_template>
inline __json_constexpr bool basic_value<__json_template_arg>::as_boolean() const
{
    if (is_boolean()) {
        if (const string_t& b_str = as_basic_type_str(); b_str == true_string<string_t>()) {
            return true;
        }
        else if (b_str == false_string<string_t>()) {
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

template <__json_template>
inline __json_constexpr int basic_value<__json_template_arg>::as_integer() const
{
    if (is_number()) {
        return std::stoi(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr unsigned basic_value<__json_template_arg>::as_unsigned() const
{
    // I don't know why there is no std::stou.
    return static_cast<unsigned>(as_unsigned_long());
}

template <__json_template>
inline __json_constexpr long basic_value<__json_template_arg>::as_long() const
{
    if (is_number()) {
        return std::stol(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr unsigned long basic_value<__json_template_arg>::as_unsigned_long() const
{
    if (is_number()) {
        return std::stoul(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr long long basic_value<__json_template_arg>::as_long_long() const
{
    if (is_number()) {
        return std::stoll(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr unsigned long long basic_value<__json_template_arg>::as_unsigned_long_long() const
{
    if (is_number()) {
        return std::stoull(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr float basic_value<__json_template_arg>::as_float() const
{
    if (is_number()) {
        return std::stof(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr double basic_value<__json_template_arg>::as_double() const
{
    if (is_number()) {
        return std::stod(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr long double basic_value<__json_template_arg>::as_long_double() const
{
    if (is_number()) {
        return std::stold(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr string_t basic_value<__json_template_arg>::as_string() const
{
    if (is_string()) {
        return as_basic_type_str();
    }
    else {
        throw exception("Wrong Type");
    }
}

template <__json_template>
inline __json_constexpr const basic_array<__json_template_arg>& basic_value<__json_template_arg>::as_array() const
{
    if (is_array()) {
        return *std::get<array_ptr>(_raw_data);
    }

    throw exception("Wrong Type");
}

template <__json_template>
inline __json_constexpr const basic_object<__json_template_arg>& basic_value<__json_template_arg>::as_object() const
{
    if (is_object()) {
        return *std::get<object_ptr>(_raw_data);
    }

    throw exception("Wrong Type or data empty");
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>& basic_value<__json_template_arg>::as_array()
{
    if (empty()) {
        *this = basic_array<__json_template_arg>();
    }

    if (is_array()) {
        return *std::get<array_ptr>(_raw_data);
    }

    throw exception("Wrong Type");
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg>& basic_value<__json_template_arg>::as_object()
{
    if (empty()) {
        *this = basic_object<__json_template_arg>();
    }

    if (is_object()) {
        return *std::get<object_ptr>(_raw_data);
    }

    throw exception("Wrong Type or data empty");
}

template <__json_template>
template <typename value_t>
inline __json_constexpr value_t basic_value<__json_template_arg>::as() const
{
    return static_cast<value_t>(*this);
}

template <__json_template>
inline __json_constexpr const string_t& basic_value<__json_template_arg>::as_basic_type_str() const
{
    return std::get<string_t>(_raw_data);
}

template <__json_template>
inline __json_constexpr string_t& basic_value<__json_template_arg>::as_basic_type_str()
{
    return std::get<string_t>(_raw_data);
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_value<__json_template_arg>::emplace(args_t&&... args)
{
    constexpr bool is_array_args =
        std::is_constructible_v<typename basic_array<__json_template_arg>::value_type, args_t...>;
    constexpr bool is_object_args =
        std::is_constructible_v<typename basic_object<__json_template_arg>::value_type, args_t...>;

    static_assert(is_array_args || is_object_args, "Args can not constructure a array or object value");

    if constexpr (is_array_args) {
        return as_array().emplace_back(std::forward<args_t>(args)...);
    }
    else if constexpr (is_object_args) {
        return as_object().emplace(std::forward<args_t>(args)...);
    }
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_value<__json_template_arg>::array_emplace(args_t&&... args)
{
    return emplace(std::forward<args_t>(args)...);
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_value<__json_template_arg>::object_emplace(args_t&&... args)
{
    return emplace(std::forward<args_t>(args)...);
}

template <__json_template>
inline __json_constexpr void basic_value<__json_template_arg>::clear() noexcept
{
    *this = basic_value<__json_template_arg>();
}

template <__json_template>
inline __json_constexpr string_t basic_value<__json_template_arg>::to_string() const
{
    switch (_type) {
    case value_type::null:
        return null_string<string_t>();
    case value_type::boolean:
    case value_type::number:
        return as_basic_type_str();
    case value_type::string:
        return char_t('"') + unescape_string(as_basic_type_str()) + char_t('"');
    case value_type::array:
        return as_array().to_string();
    case value_type::object:
        return as_object().to_string();
    default:
        throw exception("Unknown basic_value Type");
    }
}

template <__json_template>
inline __json_constexpr string_t basic_value<__json_template_arg>::format(size_t indent, size_t indent_times) const
{
    switch (_type) {
    case value_type::null:
    case value_type::boolean:
    case value_type::number:
    case value_type::string:
        return to_string();
    case value_type::array:
        return as_array().format(indent, indent_times);
    case value_type::object:
        return as_object().format(indent, indent_times);
    default:
        throw exception("Unknown basic_value Type");
    }
}

template <__json_template>
template <typename value_t>
inline __json_constexpr bool basic_value<__json_template_arg>::all() const
{
    if (is_array()) {
        return as_array().template all<value_t>();
    }
    else if (is_object()) {
        return as_object().template all<value_t>();
    }
    else {
        return false;
    }
}

template <__json_template>
template <typename value_t, template <typename...> typename vector_t>
inline __json_constexpr vector_t<value_t> basic_value<__json_template_arg>::to_vector() const
{
    return as_array().template to_vector<value_t, vector_t>();
}

template <__json_template>
template <typename value_t, template <typename...> typename another_map_t>
inline __json_constexpr another_map_t<string_t, value_t> basic_value<__json_template_arg>::to_map() const
{
    return as_object().template to_map<value_t, another_map_t>();
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator=(
    const basic_value<__json_template_arg>& rhs)
{
    _type = rhs._type;
    _raw_data = deep_copy(rhs._raw_data);

    return *this;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator=(
    basic_value<__json_template_arg>&& rhs) noexcept = default;

template <__json_template>
inline __json_constexpr bool basic_value<__json_template_arg>::operator==(
    const basic_value<__json_template_arg>& rhs) const
{
    if (_type != rhs._type) return false;

    switch (_type) {
    case value_type::null:
        return rhs.is_null();
    case value_type::boolean:
    case value_type::number:
    case value_type::string:
        return _raw_data == rhs._raw_data;
    case value_type::array:
        return as_array() == rhs.as_array();
    case value_type::object:
        return as_object() == rhs.as_object();
    default:
        throw exception("Unknown basic_value Type");
    }
}

template <__json_template>
inline __json_constexpr const basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator[](
    size_t pos) const
{
    // basic_array not support to create by operator[]

    return as_array()[pos];
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator[](size_t pos)
{
    // basic_array not support to create by operator[]

    return as_array()[pos];
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator[](
    const string_t& key)
{
    if (empty()) {
        *this = basic_object<__json_template_arg>();
    }

    return as_object()[key];
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator[](string_t&& key)
{
    if (empty()) {
        *this = basic_object<__json_template_arg>();
    }

    return as_object()[std::move(key)];
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator|(
    const basic_object<__json_template_arg>& rhs) const&
{
    return as_object() | rhs;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator|(
    basic_object<__json_template_arg>&& rhs) const&
{
    return as_object() | std::move(rhs);
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator|(
    const basic_object<__json_template_arg>& rhs) &&
{
    return std::move(as_object()) | rhs;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator|(
    basic_object<__json_template_arg>&& rhs) &&
{
    return std::move(as_object()) | std::move(rhs);
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator|=(
    const basic_object<__json_template_arg>& rhs)
{
    as_object() |= rhs;
    return *this;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator|=(
    basic_object<__json_template_arg>&& rhs)
{
    as_object() |= std::move(rhs);
    return *this;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator+(
    const basic_array<__json_template_arg>& rhs) const&
{
    return as_array() + rhs;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator+(
    basic_array<__json_template_arg>&& rhs) const&
{
    return as_array() + std::move(rhs);
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator+(
    const basic_array<__json_template_arg>& rhs) &&
{
    return std::move(as_array()) + rhs;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg> basic_value<__json_template_arg>::operator+(
    basic_array<__json_template_arg>&& rhs) &&
{
    return std::move(as_array()) + std::move(rhs);
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator+=(
    const basic_array<__json_template_arg>& rhs)
{
    as_array() += rhs;
    return *this;
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_value<__json_template_arg>::operator+=(
    basic_array<__json_template_arg>&& rhs)
{
    as_array() += std::move(rhs);
    return *this;
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr basic_value<__json_template_arg>::basic_value(value_type type, args_t&&... args)
    : _type(type), _raw_data(std::forward<args_t>(args)...)
{
    static_assert(std::is_constructible_v<var_t, args_t...>, "Parameter can't be used to construct a var_t");
}

template <__json_template>
inline __json_constexpr typename basic_value<__json_template_arg>::var_t basic_value<__json_template_arg>::deep_copy(
    const var_t& src)
{
    var_t dst;
    if (const auto string_ptr = std::get_if<string_t>(&src)) {
        dst = *string_ptr;
    }
    else if (const auto arr_ptr = std::get_if<array_ptr>(&src)) {
        dst = unique_ptr_t<basic_array<__json_template_arg>>(new basic_array<__json_template_arg>(**arr_ptr));
    }
    else if (const auto obj_ptr = std::get_if<object_ptr>(&src)) {
        dst = unique_ptr_t<basic_object<__json_template_arg>>(new basic_object<__json_template_arg>(**obj_ptr));
    }
    else {
        // maybe invalid_value
    }

    return dst;
}

// ******************************
// *      basic_array impl      *
// ******************************

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>::basic_array(std::initializer_list<value_type> init_list)
    : _array_data(init_list)
{}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>::basic_array(typename raw_array::size_type size)
    : _array_data(size)
{}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>::basic_array(const basic_value<__json_template_arg>& val)
    : basic_array<__json_template_arg>(val.as_array())
{}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>::basic_array(basic_value<__json_template_arg>&& val)
    : basic_array<__json_template_arg>(std::move(val.as_array()))
{}

template <__json_template>
inline __json_constexpr void basic_array<__json_template_arg>::clear() noexcept
{
    _array_data.clear();
}

template <__json_template>
inline __json_constexpr bool basic_array<__json_template_arg>::erase(size_t pos)
{
    return erase(_array_data.begin() + pos);
}

template <__json_template>
inline __json_constexpr bool basic_array<__json_template_arg>::erase(iterator iter)
{
    return _array_data.erase(iter) != _array_data.end();
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_array<__json_template_arg>::emplace_back(args_t&&... args)
{
    static_assert(std::is_constructible_v<value_type, args_t...>,
                  "Parameter can't be used to construct a raw_array::value_type");
    return _array_data.emplace_back(std::forward<args_t>(args)...);
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_array<__json_template_arg>::push_back(args_t&&... args)
{
    return emplace_back(std::forward<args_t>(args)...);
}

template <__json_template>
inline __json_constexpr const basic_value<__json_template_arg>& basic_array<__json_template_arg>::at(size_t pos) const
{
    return _array_data.at(pos);
}

template <__json_template>
inline __json_constexpr string_t basic_array<__json_template_arg>::to_string() const
{
    string_t str { '[' };
    for (auto iter = _array_data.cbegin(); iter != _array_data.cend();) {
        str += iter->to_string();
        if (++iter != _array_data.cend()) {
            str += ',';
        }
    }
    str += char_t(']');
    return str;
}

template <__json_template>
inline __json_constexpr string_t basic_array<__json_template_arg>::format(size_t indent, size_t indent_times) const
{
    const string_t tail_indent(indent * indent_times, ' ');
    const string_t body_indent(indent * (indent_times + 1), ' ');

    string_t str { '[', '\n' };
    for (auto iter = _array_data.cbegin(); iter != _array_data.cend();) {
        str += body_indent + iter->format(indent, indent_times + 1);
        if (++iter != _array_data.cend()) {
            str += ',';
        }
        str += '\n';
    }
    str += tail_indent + char_t(']');
    return str;
}

template <__json_template>
template <typename value_t>
inline __json_constexpr bool basic_array<__json_template_arg>::all() const
{
    for (const auto& elem : _array_data) {
        if (!elem.template is<value_t>()) {
            return false;
        }
    }
    return true;
}

namespace _to_vector_helper
{
    template <typename T>
    class has_emplace_back
    {
        template <typename U>
        static auto test(int) -> decltype(std::declval<U>().emplace_back(), std::true_type());

        template <typename U>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };
}

template <__json_template>
template <typename value_t, template <typename...> typename vector_t>
inline __json_constexpr vector_t<value_t> basic_array<__json_template_arg>::to_vector() const
{

    vector_t<value_t> result;
    if constexpr (_to_vector_helper::has_emplace_back<vector_t<value_t>>::value) {
        for (const auto& elem : _array_data) {
            result.emplace_back(elem.template as<value_t>());
        }
    }
    else {
        for (const auto& elem : _array_data) {
            result.emplace(elem.template as<value_t>());
        }
    }
    return result;
}

template <__json_template>
template <typename value_t, typename... rest_keys_t>
inline __json_constexpr auto basic_array<__json_template_arg>::get_helper(const value_t& default_value, size_t pos,
                                                                          rest_keys_t&&... rest) const
{
    constexpr bool is_json = std::is_same_v<basic_value<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_array<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_object<__json_template_arg>, value_t>;
    constexpr bool is_string = std::is_constructible_v<string_t, value_t> && !is_json;

    if (!contains(pos)) {
        if constexpr (is_string) {
            return string_t(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    return at(pos).get_helper(default_value, std::forward<rest_keys_t>(rest)...);
}

template <__json_template>
template <typename value_t>
inline __json_constexpr auto basic_array<__json_template_arg>::get_helper(const value_t& default_value,
                                                                          size_t pos) const
{
    constexpr bool is_json = std::is_same_v<basic_value<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_array<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_object<__json_template_arg>, value_t>;
    constexpr bool is_string = std::is_constructible_v<string_t, value_t> && !is_json;

    if (!contains(pos)) {
        if constexpr (is_string) {
            return string_t(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    auto val = _array_data.at(pos);
    if (val.template is<value_t>()) {
        if constexpr (is_string) {
            return val.template as<string_t>();
        }
        else {
            return val.template as<value_t>();
        }
    }
    else {
        if constexpr (is_string) {
            return string_t(default_value);
        }
        else {
            return value_t(default_value);
        }
    }
}

template <__json_template>
template <typename value_t>
inline __json_constexpr std::optional<value_t> basic_array<__json_template_arg>::find(size_t pos) const
{
    static_assert(std::is_constructible_v<value_t, basic_value<__json_template_arg>>,
                  "Type can NOT be constructed by basic_value");
    if (!contains(pos)) {
        return std::nullopt;
    }
    const auto& val = _array_data.at(pos);
    return val.template is<value_t>() ? std::optional<value_t>(val.template as<value_t>()) : std::nullopt;
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::iterator basic_array<
    __json_template_arg>::begin() noexcept
{
    return _array_data.begin();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::iterator basic_array<
    __json_template_arg>::end() noexcept
{
    return _array_data.end();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_iterator basic_array<
    __json_template_arg>::begin() const noexcept
{
    return _array_data.begin();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_iterator basic_array<
    __json_template_arg>::end() const noexcept
{
    return _array_data.end();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_iterator basic_array<
    __json_template_arg>::cbegin() const noexcept
{
    return _array_data.cbegin();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_iterator basic_array<
    __json_template_arg>::cend() const noexcept
{
    return _array_data.cend();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::reverse_iterator basic_array<
    __json_template_arg>::rbegin() noexcept
{
    return _array_data.rbegin();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::reverse_iterator basic_array<
    __json_template_arg>::rend() noexcept
{
    return _array_data.rend();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_reverse_iterator basic_array<
    __json_template_arg>::rbegin() const noexcept
{
    return _array_data.rbegin();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_reverse_iterator basic_array<
    __json_template_arg>::rend() const noexcept
{
    return _array_data.rend();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_reverse_iterator basic_array<
    __json_template_arg>::crbegin() const noexcept
{
    return _array_data.crbegin();
}

template <__json_template>
inline __json_constexpr typename basic_array<__json_template_arg>::const_reverse_iterator basic_array<
    __json_template_arg>::crend() const noexcept
{
    return _array_data.crend();
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_array<__json_template_arg>::operator[](size_t pos)
{
    return _array_data[pos];
}

template <__json_template>
inline __json_constexpr const basic_value<__json_template_arg>& basic_array<__json_template_arg>::operator[](
    size_t pos) const
{
    return _array_data[pos];
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg> basic_array<__json_template_arg>::operator+(
    const basic_array<__json_template_arg>& rhs) const&
{
    basic_array<__json_template_arg> temp = *this;
    temp._array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
    return temp;
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg> basic_array<__json_template_arg>::operator+(
    basic_array<__json_template_arg>&& rhs) const&
{
    basic_array<__json_template_arg> temp = *this;
    temp._array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()),
                            std::make_move_iterator(rhs.end()));
    return temp;
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg> basic_array<__json_template_arg>::operator+(
    const basic_array<__json_template_arg>& rhs) &&
{
    _array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
    return std::move(*this);
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg> basic_array<__json_template_arg>::operator+(
    basic_array<__json_template_arg>&& rhs) &&
{
    _array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return std::move(*this);
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>& basic_array<__json_template_arg>::operator+=(
    const basic_array<__json_template_arg>& rhs)
{
    _array_data.insert(_array_data.end(), rhs.begin(), rhs.end());
    return *this;
}

template <__json_template>
inline __json_constexpr basic_array<__json_template_arg>& basic_array<__json_template_arg>::operator+=(
    basic_array<__json_template_arg>&& rhs)
{
    _array_data.insert(_array_data.end(), std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return *this;
}

template <__json_template>
inline __json_constexpr bool basic_array<__json_template_arg>::operator==(
    const basic_array<__json_template_arg>& rhs) const
{
    return _array_data == rhs._array_data;
}

// *******************************
// *      basic_object impl      *
// *******************************

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg>::basic_object(std::initializer_list<value_type> init_list)
    : _object_data(std::make_move_iterator(init_list.begin()), std::make_move_iterator(init_list.end()))
{}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg>::basic_object(const basic_value<__json_template_arg>& val)
    : basic_object<__json_template_arg>(val.as_object())
{}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg>::basic_object(basic_value<__json_template_arg>&& val)
    : basic_object<__json_template_arg>(std::move(val.as_object()))
{}

template <__json_template>
inline __json_constexpr bool basic_object<__json_template_arg>::contains(const string_t& key) const
{
    return _object_data.find(key) != _object_data.cend();
}

template <__json_template>
inline __json_constexpr const basic_value<__json_template_arg>& basic_object<__json_template_arg>::at(
    const string_t& key) const
{
    return _object_data.at(key);
}

template <__json_template>
inline __json_constexpr void basic_object<__json_template_arg>::clear() noexcept
{
    _object_data.clear();
}

template <__json_template>
inline __json_constexpr bool basic_object<__json_template_arg>::erase(const string_t& key)
{
    return _object_data.erase(key) > 0 ? true : false;
}

template <__json_template>
inline __json_constexpr bool basic_object<__json_template_arg>::erase(iterator iter)
{
    return _object_data.erase(iter) != _object_data.end();
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_object<__json_template_arg>::emplace(args_t&&... args)
{
    static_assert(std::is_constructible_v<value_type, args_t...>,
                  "Parameter can't be used to construct a raw_object::value_type");
    return _object_data.emplace(std::forward<args_t>(args)...);
}

template <__json_template>
template <typename... args_t>
inline __json_constexpr decltype(auto) basic_object<__json_template_arg>::insert(args_t&&... args)
{
    return emplace(std::forward<args_t>(args)...);
}

template <__json_template>
inline __json_constexpr string_t basic_object<__json_template_arg>::to_string() const
{
    string_t str { '{' };
    for (auto iter = _object_data.cbegin(); iter != _object_data.cend();) {
        const auto& [key, val] = *iter;
        str += char_t('"') + unescape_string(key) + string_t { '\"', ':' } + val.to_string();
        if (++iter != _object_data.cend()) {
            str += ',';
        }
    }
    str += char_t('}');
    return str;
}

template <__json_template>
inline __json_constexpr string_t basic_object<__json_template_arg>::format(size_t indent, size_t indent_times) const
{
    const string_t tail_indent(indent * indent_times, ' ');
    const string_t body_indent(indent * (indent_times + 1), ' ');

    string_t str { '{', '\n' };
    for (auto iter = _object_data.cbegin(); iter != _object_data.cend();) {
        const auto& [key, val] = *iter;
        str += body_indent + char_t('"') + unescape_string(key) + string_t { '\"', ':', ' ' } +
               val.format(indent, indent_times + 1);
        if (++iter != _object_data.cend()) {
            str += ',';
        }
        str += '\n';
    }
    str += tail_indent + char_t('}');
    return str;
}

template <__json_template>
template <typename value_t>
inline __json_constexpr bool basic_object<__json_template_arg>::all() const
{
    for (const auto& [_, val] : _object_data) {
        if (!val.template is<value_t>()) {
            return false;
        }
    }
    return true;
}

template <__json_template>
template <typename value_t, template <typename...> typename another_map_t>
inline __json_constexpr another_map_t<string_t, value_t> basic_object<__json_template_arg>::to_map() const
{
    another_map_t<string_t, value_t> result;
    for (const auto& [key, val] : _object_data) {
        result.emplace(key, val.template as<value_t>());
    }
    return result;
}

template <__json_template>
template <typename value_t, typename... rest_keys_t>
inline __json_constexpr auto basic_object<__json_template_arg>::get_helper(const value_t& default_value,
                                                                           const string_t& key,
                                                                           rest_keys_t&&... rest) const
{
    constexpr bool is_json = std::is_same_v<basic_value<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_array<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_object<__json_template_arg>, value_t>;
    constexpr bool is_string = std::is_constructible_v<string_t, value_t> && !is_json;

    if (!contains(key)) {
        if constexpr (is_string) {
            return string_t(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    return at(key).get_helper(default_value, std::forward<rest_keys_t>(rest)...);
}

template <__json_template>
template <typename value_t>
inline __json_constexpr auto basic_object<__json_template_arg>::get_helper(const value_t& default_value,
                                                                           const string_t& key) const
{
    constexpr bool is_json = std::is_same_v<basic_value<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_array<__json_template_arg>, value_t> ||
                             std::is_same_v<basic_object<__json_template_arg>, value_t>;
    constexpr bool is_string = std::is_constructible_v<string_t, value_t> && !is_json;

    if (!contains(key)) {
        if constexpr (is_string) {
            return string_t(default_value);
        }
        else {
            return value_t(default_value);
        }
    }

    auto val = _object_data.at(key);
    if (val.template is<value_t>()) {
        if constexpr (is_string) {
            return val.template as<string_t>();
        }
        else {
            return val.template as<value_t>();
        }
    }
    else {
        if constexpr (is_string) {
            return string_t(default_value);
        }
        else {
            return value_t(default_value);
        }
    }
}

template <__json_template>
template <typename value_t>
inline __json_constexpr std::optional<value_t> basic_object<__json_template_arg>::find(const string_t& key) const
{
    static_assert(std::is_constructible_v<value_t, basic_value<__json_template_arg>>,
                  "value_t can NOT be constructed by basic_value");
    auto iter = _object_data.find(key);
    if (iter == _object_data.end()) {
        return std::nullopt;
    }
    const auto& val = iter->second;
    return val.template is<value_t>() ? std::optional<value_t>(val.template as<value_t>()) : std::nullopt;
}

template <__json_template>
inline __json_constexpr typename basic_object<__json_template_arg>::iterator basic_object<
    __json_template_arg>::begin() noexcept
{
    return _object_data.begin();
}

template <__json_template>
inline __json_constexpr typename basic_object<__json_template_arg>::iterator basic_object<
    __json_template_arg>::end() noexcept
{
    return _object_data.end();
}

template <__json_template>
inline __json_constexpr typename basic_object<__json_template_arg>::const_iterator basic_object<
    __json_template_arg>::begin() const noexcept
{
    return _object_data.begin();
}

template <__json_template>
inline __json_constexpr typename basic_object<__json_template_arg>::const_iterator basic_object<
    __json_template_arg>::end() const noexcept
{
    return _object_data.end();
}

template <__json_template>
inline __json_constexpr typename basic_object<__json_template_arg>::const_iterator basic_object<
    __json_template_arg>::cbegin() const noexcept
{
    return _object_data.cbegin();
}

template <__json_template>
inline __json_constexpr typename basic_object<__json_template_arg>::const_iterator basic_object<
    __json_template_arg>::cend() const noexcept
{
    return _object_data.cend();
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_object<__json_template_arg>::operator[](
    const string_t& key)
{
    return _object_data[key];
}

template <__json_template>
inline __json_constexpr basic_value<__json_template_arg>& basic_object<__json_template_arg>::operator[](string_t&& key)
{
    return _object_data[std::move(key)];
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg> basic_object<__json_template_arg>::operator|(
    const basic_object<__json_template_arg>& rhs) const&
{
    basic_object<__json_template_arg> temp = *this;
    temp._object_data.insert(rhs.begin(), rhs.end());
    return temp;
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg> basic_object<__json_template_arg>::operator|(
    basic_object<__json_template_arg>&& rhs) const&
{
    basic_object<__json_template_arg> temp = *this;
    // temp._object_data.merge(std::move(rhs._object_data));
    temp._object_data.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return temp;
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg> basic_object<__json_template_arg>::operator|(
    const basic_object<__json_template_arg>& rhs) &&
{
    _object_data.insert(rhs.begin(), rhs.end());
    return std::move(*this);
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg> basic_object<__json_template_arg>::operator|(
    basic_object<__json_template_arg>&& rhs) &&
{
    //_object_data.merge(std::move(rhs._object_data));
    _object_data.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return std::move(*this);
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg>& basic_object<__json_template_arg>::operator|=(
    const basic_object<__json_template_arg>& rhs)
{
    _object_data.insert(rhs.begin(), rhs.end());
    return *this;
}

template <__json_template>
inline __json_constexpr basic_object<__json_template_arg>& basic_object<__json_template_arg>::operator|=(
    basic_object<__json_template_arg>&& rhs)
{
    _object_data.insert(std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()));
    return *this;
}

template <__json_template>
inline __json_constexpr bool basic_object<__json_template_arg>::operator==(
    const basic_object<__json_template_arg>& rhs) const
{
    return _object_data == rhs._object_data;
}

// *************************
// *      parser impl      *
// *************************

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr std::optional<basic_value<__json_template_arg>> parser<
    __json_template_arg, parsing_t, accel_traits>::parse(const parsing_t& content)
{
    return parser<__json_template_arg, parsing_t, accel_traits>(content.cbegin(), content.cend()).parse();
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr std::optional<basic_value<__json_template_arg>> parser<__json_template_arg, parsing_t,
                                                                               accel_traits>::parse()
{
    if (!skip_whitespace()) {
        return std::nullopt;
    }

    basic_value<__json_template_arg> result_value;
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

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_value()
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
        return invalid_value<__json_template_arg>();
    }
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_null()
{
    for (const auto& ch : null_string<string_t>()) {
        if (_cur != _end && *_cur == ch) {
            ++_cur;
        }
        else {
            return invalid_value<__json_template_arg>();
        }
    }

    return basic_value<__json_template_arg>();
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_boolean()
{
    switch (*_cur) {
    case 't':
        for (const auto& ch : true_string<string_t>()) {
            if (_cur != _end && *_cur == ch) {
                ++_cur;
            }
            else {
                return invalid_value<__json_template_arg>();
            }
        }
        return true;
    case 'f':
        for (const auto& ch : false_string<string_t>()) {
            if (_cur != _end && *_cur == ch) {
                ++_cur;
            }
            else {
                return invalid_value<__json_template_arg>();
            }
        }
        return false;
    default:
        return invalid_value<__json_template_arg>();
    }
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_number()
{
    const auto first = _cur;
    if (*_cur == '-') {
        ++_cur;
    }

    // numbers cannot have leading zeroes
    if (_cur != _end && *_cur == '0' && _cur + 1 != _end && std::isdigit(*(_cur + 1))) {
        return invalid_value<__json_template_arg>();
    }

    if (!skip_digit()) {
        return invalid_value<__json_template_arg>();
    }

    if (*_cur == '.') {
        ++_cur;
        if (!skip_digit()) {
            return invalid_value<__json_template_arg>();
        }
    }

    if (*_cur == 'e' || *_cur == 'E') {
        if (++_cur == _end) {
            return invalid_value<__json_template_arg>();
        }
        if (*_cur == '+' || *_cur == '-') {
            ++_cur;
        }
        if (!skip_digit()) {
            return invalid_value<__json_template_arg>();
        }
    }

    return basic_value<__json_template_arg>(basic_value<__json_template_arg>::value_type::number,
                                            string_t(first, _cur));
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_string()
{
    auto string_opt = parse_stdstring();
    if (!string_opt) {
        return invalid_value<__json_template_arg>();
    }
    return basic_value<__json_template_arg>(basic_value<__json_template_arg>::value_type::string,
                                            std::move(string_opt).value());
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_array()
{
    if (*_cur == '[') {
        ++_cur;
    }
    else {
        return invalid_value<__json_template_arg>();
    }

    if (!skip_whitespace()) {
        return invalid_value<__json_template_arg>();
    }
    else if (*_cur == ']') {
        ++_cur;
        // empty basic_array
        return basic_array<__json_template_arg>();
    }

    typename basic_array<__json_template_arg>::raw_array result;
    while (true) {
        if (!skip_whitespace()) {
            return invalid_value<__json_template_arg>();
        }

        basic_value<__json_template_arg> val = parse_value();

        if (!val.valid() || !skip_whitespace()) {
            return invalid_value<__json_template_arg>();
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
        return invalid_value<__json_template_arg>();
    }

    return basic_array<__json_template_arg>(std::move(result));
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr basic_value<__json_template_arg> parser<__json_template_arg, parsing_t,
                                                                accel_traits>::parse_object()
{
    if (*_cur == '{') {
        ++_cur;
    }
    else {
        return invalid_value<__json_template_arg>();
    }

    if (!skip_whitespace()) {
        return invalid_value<__json_template_arg>();
    }
    else if (*_cur == '}') {
        ++_cur;
        // empty basic_object
        return basic_object<__json_template_arg>();
    }

    typename basic_object<__json_template_arg>::raw_object result;
    while (true) {
        if (!skip_whitespace()) {
            return invalid_value<__json_template_arg>();
        }

        auto key_opt = parse_stdstring();

        if (key_opt && skip_whitespace() && *_cur == ':') {
            ++_cur;
        }
        else {
            return invalid_value<__json_template_arg>();
        }

        if (!skip_whitespace()) {
            return invalid_value<__json_template_arg>();
        }

        basic_value<__json_template_arg> val = parse_value();

        if (!val.valid() || !skip_whitespace()) {
            return invalid_value<__json_template_arg>();
        }

        result.emplace(std::move(*key_opt), std::move(val));

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
        return invalid_value<__json_template_arg>();
    }

    return basic_object<__json_template_arg>(std::move(result));
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr std::optional<string_t> parser<__json_template_arg, parsing_t, accel_traits>::parse_stdstring()
{
    if (*_cur == '"') {
        ++_cur;
    }
    else {
        return std::nullopt;
    }

    string_t result;
    auto no_escape_beg = _cur;

    while (_cur != _end) {
        if constexpr (sizeof(*_cur) == 1 && accel_traits::available) {
            if (!skip_string_literal_with_accel()) {
                return std::nullopt;
            }
        }
        switch (*_cur) {
        case '\t':
        case '\r':
        case '\n':
            return std::nullopt;
        case '\\': {
            result += string_t(no_escape_beg, _cur++);
            if (_cur == _end) {
                return std::nullopt;
            }
            switch (*_cur) {
            case '"':
                result.push_back('"');
                break;
            case '\\':
                result.push_back('\\');
                break;
            case '/':
                result.push_back('/');
                break;
            case 'b':
                result.push_back('\b');
                break;
            case 'f':
                result.push_back('\f');
                break;
            case 'n':
                result.push_back('\n');
                break;
            case 'r':
                result.push_back('\r');
                break;
            case 't':
                result.push_back('\t');
                break;
                // case 'u':
                //     result.push_back('\u');
                //     break;
            default:
                // Illegal backslash escape
                return std::nullopt;
            }
            no_escape_beg = ++_cur;
            break;
        }
        case '"': {
            result += string_t(no_escape_beg, _cur++);
            return result;
        }
        default:
            ++_cur;
            break;
        }
    }
    return std::nullopt;
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr bool parser<__json_template_arg, parsing_t, accel_traits>::skip_string_literal_with_accel()
{
    if constexpr (sizeof(*_cur) != 1) {
        return false;
    }

    while (_end - _cur >= accel_traits::step) {
        auto pack = accel_traits::load_unaligned(&(*_cur));
        auto result = accel_traits::less(pack, 32);
        result = accel_traits::bitwise_or(result, accel_traits::equal(pack, static_cast<uint8_t>('"')));
        result = accel_traits::bitwise_or(result, accel_traits::equal(pack, static_cast<uint8_t>('\\')));

        if (accel_traits::is_all_zero(result)) {
            _cur += accel_traits::step;
        }
        else {
            auto index = accel_traits::first_nonzero_byte(result);
            _cur += index;
            break;
        }
    }

    return _cur != _end;
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr bool parser<__json_template_arg, parsing_t, accel_traits>::skip_whitespace() noexcept
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

template <typename elem_t>
__json_constexpr bool wrap_is_digit(elem_t ch)
{
#ifdef __json_enable_constexpr
    if (std::is_constant_evaluated()) {
        return soft_is_digit(ch);
    }
#endif
    return std::isdigit(ch);
}

template <__json_template, typename parsing_t, typename accel_traits>
inline __json_constexpr bool parser<__json_template_arg, parsing_t, accel_traits>::skip_digit()
{
    // At least one digit
    if (_cur != _end && wrap_is_digit(*_cur)) {
        ++_cur;
    }
    else {
        return false;
    }

    while (_cur != _end && wrap_is_digit(*_cur)) {
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
// *      utils impl       *
// *************************

template <typename parsing_t>
__json_constexpr auto parse(const parsing_t& content)
{
    using string_t = std::basic_string<typename parsing_t::value_type>;
    return parser<string_t, default_map_t, default_unique_ptr_t, default_variant_t, parsing_t>::parse(content);
}

#ifdef __json_enable_constexpr
template <typename parsing_t>
__json_constexpr auto cparse(const parsing_t& content)
{
    using string_t = std::basic_string<typename parsing_t::value_type>;
    return parser<string_t, constexpr_map, constexpr_unique_ptr, constexpr_variant, parsing_t>::parse(content);
}
#endif

template <typename char_t>
__json_constexpr auto parse(char_t* content)
{
    return parse(std::basic_string_view<std::decay_t<char_t>> { content });
}

template <typename istream_t, typename _>
__json_constexpr auto parse(istream_t& ifs, bool check_bom)
{
    using string_t = std::basic_string<typename istream_t::char_type>;

    ifs.seekg(0, std::ios::end);
    auto file_size = ifs.tellg();

    ifs.seekg(0, std::ios::beg);
    string_t str(file_size, '\0');

    ifs.read(str.data(), file_size);

    if (check_bom) {
        using uchar = unsigned char;
        constexpr uchar Bom_0 = 0xEF;
        constexpr uchar Bom_1 = 0xBB;
        constexpr uchar Bom_2 = 0xBF;

        if (str.size() >= 3 && static_cast<uchar>(str.at(0)) == Bom_0 && static_cast<uchar>(str.at(1)) == Bom_1 &&
            static_cast<uchar>(str.at(2)) == Bom_2) {
            str.assign(str.begin() + 3, str.end());
        }
    }
    return parse(str);
}

template <typename ifstream_t, typename path_t>
auto open(const path_t& filepath, bool check_bom)
{
    using char_t = typename ifstream_t::char_type;
    using string_t = std::basic_string<char_t>;
    using json_t = json::basic_value<string_t, default_map_t, default_unique_ptr_t, default_variant_t>;
    using return_t = std::optional<json_t>;

    ifstream_t ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return return_t(std::nullopt);
    }
    auto opt = parse(ifs, check_bom);
    ifs.close();
    return opt;
}

template <typename ostream_t, __json_template,
          typename std_ostream_t =
              std::basic_ostream<typename string_t::value_type, std::char_traits<typename string_t::value_type>>,
          typename enable_t = typename std::enable_if_t<std::is_same_v<std_ostream_t, ostream_t> ||
                                                        std::is_base_of_v<std_ostream_t, ostream_t>>>
ostream_t& operator<<(ostream_t& out, const basic_value<__json_template_arg>& val)
{
    out << val.format();
    return out;
}
template <typename ostream_t, __json_template,
          typename std_ostream_t =
              std::basic_ostream<typename string_t::value_type, std::char_traits<typename string_t::value_type>>,
          typename enable_t =
              std::enable_if_t<std::is_same_v<std_ostream_t, ostream_t> || std::is_base_of_v<std_ostream_t, ostream_t>>>
ostream_t& operator<<(ostream_t& out, const basic_array<__json_template_arg>& arr)
{
    out << arr.format();
    return out;
}
template <typename ostream_t, __json_template,
          typename std_ostream_t =
              std::basic_ostream<typename string_t::value_type, std::char_traits<typename string_t::value_type>>,
          typename enable_t =
              std::enable_if_t<std::is_same_v<std_ostream_t, ostream_t> || std::is_base_of_v<std_ostream_t, ostream_t>>>
ostream_t& operator<<(ostream_t& out, const basic_object<__json_template_arg>& obj)
{
    out << obj.format();
    return out;
}

namespace literals
{
    inline value operator""_json(const char* str, size_t len)
    {
        return operator""_jvalue(str, len);
    }
    inline wvalue operator""_json(const wchar_t* str, size_t len)
    {
        return operator""_jvalue(str, len);
    }

    inline value operator""_jvalue(const char* str, size_t len)
    {
        return parse(std::string_view(str, len)).value_or(value());
    }
    inline wvalue operator""_jvalue(const wchar_t* str, size_t len)
    {
        return parse(std::wstring_view(str, len)).value_or(wvalue());
    }

    inline array operator""_jarray(const char* str, size_t len)
    {
        auto val = parse(std::string_view(str, len)).value_or(value());
        return val.is_array() ? val.as_array() : array();
    }
    inline warray operator""_jarray(const wchar_t* str, size_t len)
    {
        auto val = parse(std::wstring_view(str, len)).value_or(wvalue());
        return val.is_array() ? val.as_array() : warray();
    }

    inline object operator""_jobject(const char* str, size_t len)
    {
        auto val = parse(std::string_view(str, len)).value_or(value());
        return val.is_object() ? val.as_object() : object();
    }
    inline wobject operator""_jobject(const wchar_t* str, size_t len)
    {
        auto val = parse(std::wstring_view(str, len)).value_or(wvalue());
        return val.is_object() ? val.as_object() : wobject();
    }

#ifdef __json_enable_constexpr
    inline __json_constexpr cvalue operator""_cjson(const char* str, size_t len)
    {
        return operator""_cjvalue(str, len);
    }

    inline __json_constexpr cvalue operator""_cjvalue(const char* str, size_t len)
    {
        return cparse(std::string_view(str, len)).value_or(cvalue());
    }

    inline __json_constexpr carray operator""_cjarray(const char* str, size_t len)
    {
        auto val = cparse(std::string_view(str, len)).value_or(cvalue());
        return val.is_array() ? val.as_array() : carray();
    }

    inline __json_constexpr cobject operator""_cjobject(const char* str, size_t len)
    {
        auto val = cparse(std::string_view(str, len)).value_or(cvalue());
        return val.is_object() ? val.as_object() : cobject();
    }
#endif
} // namespace literals

template <__json_template>
__json_constexpr const basic_value<__json_template_arg> invalid_value()
{
    return basic_value<__json_template_arg>(basic_value<__json_template_arg>::value_type::invalid,
                                            typename basic_value<__json_template_arg>::var_t());
}

namespace _serialization_helper
{
    template <typename char_t, typename T>
    class has_output_operator
    {
        using ostringstream_t = std::basic_ostringstream<char_t, std::char_traits<char_t>, std::allocator<char_t>>;

        template <typename U>
        static auto test(int) -> decltype(std::declval<ostringstream_t&>() << std::declval<U>(), std::true_type());

        template <typename U>
        static std::false_type test(...);

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    template <typename T, typename = void>
    constexpr bool is_container = false;
    template <typename T>
    constexpr bool is_container<T, std::void_t<typename T::value_type, utils::range_value_t<T>>> =
        std::is_same_v<typename T::value_type, utils::range_value_t<T>>;

    template <typename T, typename = void>
    constexpr bool is_map = false;
    template <typename T>
    constexpr bool is_map<T, std::void_t<typename T::key_type, typename T::mapped_type>> = is_container<T>;

    template <typename T, typename = void>
    constexpr bool is_collection = false;
    template <typename T>
    constexpr bool is_collection<T> = is_container<T> && !is_map<T>;

    template <bool loose, typename string_t>
    struct string_converter
    {
        using char_t = typename string_t::value_type;
        using ostringstream_t = std::basic_ostringstream<char_t, std::char_traits<char_t>, std::allocator<char_t>>;

        template <typename input_t>
        static constexpr bool is_convertible =
            std::is_constructible_v<string_t, input_t> || (loose && has_output_operator<char_t, input_t>::value);

        template <typename input_t>
        string_t operator()(input_t&& arg) const
        {
            if constexpr (std::is_constructible_v<string_t, input_t>) {
                return string_t(std::forward<input_t>(arg));
            }
            else if constexpr (!loose) {
                static_assert(!sizeof(input_t), "Unable to convert type to string.");
            }
            else if constexpr (has_output_operator<char_t, input_t>::value) {
                ostringstream_t os;
                os << std::forward<input_t>(arg);
                return std::move(os).str();
            }
            else {
                return serialize<loose, input_t, string_t>(std::forward<input_t>(arg), *this).dumps();
            }
        }
    };

    template <typename T>
    void unable_to_serialize()
    {
        static_assert(!sizeof(T), "Unable to serialize T. "
                                  "You can define the conversion of T to json, or overload operator<< for it. "
#ifdef _MSC_VER
                                  "See T below: " __FUNCSIG__
#else
        //"See T below: " __PRETTY_FUNCTION__

#endif
        );
    }
} // namespace _serialization_helper

template <bool loose, typename any_t, __json_template, typename string_converter_t>
__json_constexpr basic_value<__json_template_arg> serialize(any_t&& arg, string_converter_t&& string_converter)
{
    using namespace _serialization_helper;

    if constexpr (std::is_constructible_v<basic_value<__json_template_arg>, any_t>) {
        return basic_value<__json_template_arg>(std::forward<any_t>(arg));
    }
    else if constexpr (std::is_constructible_v<basic_array<__json_template_arg>, any_t>) {
        return basic_array<__json_template_arg>(std::forward<any_t>(arg));
    }
    else if constexpr (std::is_constructible_v<basic_object<__json_template_arg>, any_t>) {
        return basic_object<__json_template_arg>(std::forward<any_t>(arg));
    }
    else if constexpr (std::decay_t<string_converter_t>::template is_convertible<any_t>) {
        return string_converter(std::forward<any_t>(arg));
    }
    else if constexpr (is_collection<std::decay_t<any_t>>) {
        basic_value<__json_template_arg> result;
        for (auto&& val : arg) {
            using value_t = decltype(val);

            result.emplace(serialize<loose, value_t, string_t>(std::forward<value_t>(val),
                                                               std::forward<string_converter_t>(string_converter)));
        }
        return result;
    }
    else if constexpr (is_map<std::decay_t<any_t>>) {
        basic_value<__json_template_arg> result;
        for (auto&& [key, val] : arg) {
            using key_t = decltype(key);
            using value_t = decltype(val);

            result.emplace(string_converter(std::forward<key_t>(key)),
                           serialize<loose, value_t, string_t>(std::forward<value_t>(val),
                                                               std::forward<string_converter_t>(string_converter)));
        }
        return result;
    }
    else {
        unable_to_serialize<any_t>();
    }
}

template <typename string_t>
static constexpr string_t unescape_string(const string_t& str)
{
    using char_t = typename string_t::value_type;

    string_t result;
    auto cur = str.cbegin();
    auto end = str.cend();
    auto no_escape_beg = cur;
    char_t escape = 0;

    for (; cur != end; ++cur) {
        switch (*cur) {
        case '"':
            escape = '"';
            break;
        case '\\':
            escape = '\\';
            break;
        case '\b':
            escape = 'b';
            break;
        case '\f':
            escape = 'f';
            break;
        case '\n':
            escape = 'n';
            break;
        case '\r':
            escape = 'r';
            break;
        case '\t':
            escape = 't';
            break;
        default:
            break;
        }
        if (escape) {
            result += string_t(no_escape_beg, cur) + char_t('\\') + escape;
            no_escape_beg = cur + 1;
            escape = 0;
        }
    }
    result += string_t(no_escape_beg, cur);

    return result;
}
} // namespace json
