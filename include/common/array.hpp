// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <initializer_list>
#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "exception.hpp"
#include "utils.hpp"

namespace json
{
class array
{
    friend class value;
    friend class object;

public:
    using raw_array = std::vector<value>;
    using value_type = typename raw_array::value_type;
    using iterator = typename raw_array::iterator;
    using const_iterator = typename raw_array::const_iterator;
    using reverse_iterator = typename raw_array::reverse_iterator;
    using const_reverse_iterator = typename raw_array::const_reverse_iterator;

public:
    array();
    array(const array& rhs);
    array(array&& rhs) noexcept;
    array(std::initializer_list<value_type> init_list);
    array(typename raw_array::size_type size);

    // explicit array(const value& val);
    // explicit array(value&& val);

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, array> && _utils::has_to_json_in_member<jsonization_t>::value,
            bool> = true>
    array(const jsonization_t& val)
        : array(val.to_json())
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, array> && _utils::has_to_json_in_templ_spec<std::decay_t<jsonization_t>>::value,
            bool> = true>
    array(const jsonization_t& val)
        : array(ext::jsonization<std::decay_t<jsonization_t>>().to_json(val))
    {
    }

    template <
        typename jsonization_t,
        std::enable_if_t<
            !std::is_same_v<std::decay_t<jsonization_t>, array> && std::is_rvalue_reference_v<jsonization_t&&>
                && _utils::has_move_to_json_in_templ_spec<std::decay_t<jsonization_t>>::value,
            bool> = true>
    array(jsonization_t&& val)
        : array(ext::jsonization<std::decay_t<jsonization_t>>().move_to_json(std::move(val)))
    {
    }

    // Native support for fixed-size arrays (std::array<T, N>)
    template <
        template <typename, size_t> typename arr_t,
        typename value_t,
        size_t size,
        std::enable_if_t<
            _utils::is_fixed_array<arr_t<value_t, size>> && !std::is_same_v<std::decay_t<arr_t<value_t, size>>, array>
                && !_utils::has_to_json_in_member<arr_t<value_t, size>>::value
                && !_utils::has_to_json_in_templ_spec<arr_t<value_t, size>>::value,
            bool> = true>
    array(const arr_t<value_t, size>& arr)
    {
        for (size_t i = 0; i < size; i++) {
            _array_data.emplace_back(arr.at(i));
        }
    }

    template <
        template <typename, size_t> typename arr_t,
        typename value_t,
        size_t size,
        std::enable_if_t<
            _utils::is_fixed_array<arr_t<value_t, size>> && !std::is_same_v<std::decay_t<arr_t<value_t, size>>, array>
                && !_utils::has_to_json_in_member<arr_t<value_t, size>>::value
                && !_utils::has_to_json_in_templ_spec<arr_t<value_t, size>>::value,
            bool> = true>
    array(arr_t<value_t, size>&& arr)
    {
        for (size_t i = 0; i < size; i++) {
            _array_data.emplace_back(std::move(arr.at(i)));
        }
    }

    // Native support for collections (std::vector, std::list, std::set, etc.)
    template <
        typename collection_t,
        std::enable_if_t<
            _utils::is_collection<collection_t> && !std::is_same_v<std::decay_t<collection_t>, array>
                && !_utils::has_to_json_in_member<collection_t>::value && !_utils::has_to_json_in_templ_spec<collection_t>::value,
            bool> = true>
    array(const collection_t& coll)
    {
        for (const auto& val : coll) {
            _array_data.emplace_back(val);
        }
    }

    template <
        typename collection_t,
        std::enable_if_t<
            _utils::is_collection<collection_t> && !std::is_same_v<std::decay_t<collection_t>, array>
                && !_utils::has_to_json_in_member<collection_t>::value && !_utils::has_to_json_in_templ_spec<collection_t>::value,
            bool> = true>
    array(collection_t&& coll)
    {
        for (auto& val : coll) {
            _array_data.emplace_back(std::move(val));
        }
    }

    // Native support for tuple-like types (std::tuple, std::pair)
    template <
        template <typename...> typename tuple_t,
        typename... args_t,
        std::enable_if_t<
            _utils::is_tuple_like<tuple_t<args_t...>> && !std::is_same_v<std::decay_t<tuple_t<args_t...>>, array>
                && !_utils::has_to_json_in_member<tuple_t<args_t...>>::value
                && !_utils::has_to_json_in_templ_spec<tuple_t<args_t...>>::value,
            bool> = true>
    array(const tuple_t<args_t...>& tpl)
    {
        constexpr size_t tuple_size = std::tuple_size_v<tuple_t<args_t...>>;
        construct_from_tuple_helper(tpl, std::make_index_sequence<tuple_size>());
    }

    template <
        template <typename...> typename tuple_t,
        typename... args_t,
        std::enable_if_t<
            _utils::is_tuple_like<tuple_t<args_t...>> && !std::is_same_v<std::decay_t<tuple_t<args_t...>>, array>
                && !_utils::has_to_json_in_member<tuple_t<args_t...>>::value
                && !_utils::has_to_json_in_templ_spec<tuple_t<args_t...>>::value,
            bool> = true>
    array(tuple_t<args_t...>&& tpl)
    {
        constexpr size_t tuple_size = std::tuple_size_v<tuple_t<args_t...>>;
        construct_from_tuple_move_helper(std::move(tpl), std::make_index_sequence<tuple_size>());
    }

    ~array() noexcept;

    bool empty() const noexcept;

    size_t size() const noexcept;

    bool contains(size_t pos) const;

    bool exists(size_t pos) const;

    const value& at(size_t pos) const;

    std::string dumps(std::optional<size_t> indent = std::nullopt) const;

    std::string to_string() const;

    std::string format(size_t indent = 4) const;

    template <typename value_t>
    bool all() const;

    // Unified as function for converting to fixed-size arrays, collections, and tuple-like types
    template <typename T>
    T as() const&;

    template <typename T>
    T as() &&;

private:
    template <typename tuple_t, size_t... Is>
    void construct_from_tuple_helper(const tuple_t& tpl, std::index_sequence<Is...>)
    {
        using std::get;
        (_array_data.emplace_back(get<Is>(tpl)), ...);
    }

    template <typename tuple_t, size_t... Is>
    void construct_from_tuple_move_helper(tuple_t&& tpl, std::index_sequence<Is...>)
    {
        using std::get;
        (_array_data.emplace_back(std::move(get<Is>(tpl))), ...);
    }

    template <typename tuple_t, size_t... Is>
    void as_tuple_helper(tuple_t& result, std::index_sequence<Is...>) const;

    template <typename tuple_t, size_t... Is>
    void move_as_tuple_helper(tuple_t& result, std::index_sequence<Is...>);

public:

    // Usage: get(key_1, key_2, ..., default_value);
    template <typename... key_then_default_value_t>
    auto get(key_then_default_value_t&&... keys_then_default_value) const;

    template <typename value_t = value>
    std::optional<value_t> find(size_t pos) const;

    template <typename... args_t>
    decltype(auto) emplace_back(args_t&&... args);
    template <typename... args_t>
    decltype(auto) push_back(args_t&&... args);

    void clear() noexcept;
    bool erase(size_t pos);
    bool erase(iterator iter);

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

    array operator+(const array& rhs) const&;
    array operator+(array&& rhs) const&;
    array operator+(const array& rhs) &&;
    array operator+(array&& rhs) &&;

    array& operator+=(const array& rhs);
    array& operator+=(array&& rhs);

    array& operator=(const array&);
    array& operator=(array&&) noexcept;

    template <typename value_t, std::enable_if_t<std::is_convertible_v<value_t, array>, bool> = true>
    array& operator=(value_t rhs)
    {
        return *this = array(std::move(rhs));
    }

    bool operator==(const array& rhs) const;

    bool operator!=(const array& rhs) const;

    template <typename jsonization_t, std::enable_if_t<_utils::has_from_json_in_member<jsonization_t>::value, bool> = true>
    explicit operator jsonization_t() const&
    {
        jsonization_t dst {};
        if (!dst.from_json(*this)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <
        typename jsonization_t,
        std::enable_if_t<_utils::has_from_json_in_templ_spec<std::decay_t<jsonization_t>>::value, bool> = true>
    explicit operator jsonization_t() const&
    {
        jsonization_t dst {};
        if (!ext::jsonization<std::decay_t<jsonization_t>>().from_json(*this, dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    template <
        typename jsonization_t,
        std::enable_if_t<_utils::has_move_from_json_in_templ_spec<std::decay_t<jsonization_t>>::value, bool> = true>
    explicit operator jsonization_t() &&
    {
        jsonization_t dst {};
        if (!ext::jsonization<std::decay_t<jsonization_t>>().move_from_json(std::move(*this), dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }

    // Unified native support for converting to fixed-size arrays, collections, and tuple-like types
    template <
        typename T,
        std::enable_if_t<
            (_utils::is_fixed_array<T> || _utils::is_collection<T> || _utils::is_tuple_like<T>)
                && !_utils::has_from_json_in_member<T>::value && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() const&
    {
        return as<T>();
    }

    template <
        typename T,
        std::enable_if_t<
            (_utils::is_fixed_array<T> || _utils::is_collection<T> || _utils::is_tuple_like<T>)
                && !_utils::has_from_json_in_member<T>::value && !_utils::has_from_json_in_templ_spec<T>::value,
            bool> = true>
    explicit operator T() &&
    {
        return std::move(*this).as<T>();
    }

private:
    template <typename... key_then_default_value_t, size_t... keys_indexes_t>
    auto get(std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const;
    template <typename value_t, typename... rest_keys_t>
    auto get_helper(const value_t& default_value, size_t pos, rest_keys_t&&... rest) const;
    template <typename value_t>
    auto get_helper(const value_t& default_value, size_t pos) const;

    std::string format(size_t indent, size_t indent_times) const;

private:
    raw_array _array_data;
};
} // namespace json
