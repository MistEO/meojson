#pragma once

#include "value.hpp"

namespace json
{
inline value::value() = default;

inline value::value(const value& rhs)
    : _type(rhs._type)
    , _raw_data(deep_copy(rhs._raw_data))
{
}

inline value::value(value&& rhs) noexcept = default;

inline value::value(bool b)
    : _type(value_type::boolean)
    , _raw_data(b ? std::string(_utils::true_string()) : std::string(_utils::false_string()))
{
}

inline value::value(int num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(unsigned num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(long num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(unsigned long num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(long long num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(unsigned long long num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(float num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(double num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(long double num)
    : _type(value_type::number)
    , _raw_data(_utils::to_basic_string(num))
{
}

inline value::value(const char* str)
    : _type(value_type::string)
    , _raw_data(std::string(str))
{
}

inline value::value(std::string str)
    : _type(value_type::string)
    , _raw_data(std::move(str))
{
}

inline value::value(std::string_view str)
    : _type(value_type::string)
    , _raw_data(std::string(str))
{
}

inline value::value(std::nullptr_t)
    : _type(value_type::null)
{
}

inline value::value(std::monostate)
    : _type(value_type::null)
{
}

inline value::value(const array& arr)
    : _type(value_type::array)
    , _raw_data(std::make_unique<array>(arr))
{
}

inline value::value(array&& arr)
    : _type(value_type::array)
    , _raw_data(std::make_unique<array>(std::move(arr)))
{
}

inline value::value(const object& obj)
    : _type(value_type::object)
    , _raw_data(std::make_unique<object>(obj))
{
}

inline value::value(object&& obj)
    : _type(value_type::object)
    , _raw_data(std::make_unique<object>(std::move(obj)))
{
}

inline value::value(std::initializer_list<typename object::value_type> init_list)
    : _type(value_type::object)
    , _raw_data(std::make_unique<object>(init_list))
{
}

inline value::~value() = default;

inline bool value::valid() const noexcept
{
    return _type != value_type::invalid;
}

inline bool value::empty() const noexcept
{
    switch (_type) {
    case value_type::null:
        return true;
    case value_type::string:
        return std::get<std::string>(_raw_data).empty();
    case value_type::array:
        return std::get<array_ptr>(_raw_data)->empty();
    case value_type::object:
        return std::get<object_ptr>(_raw_data)->empty();
    default:
        return false;
    }
}

inline bool value::is_null() const noexcept
{
    return _type == value_type::null;
}

inline bool value::is_number() const noexcept
{
    return _type == value_type::number;
}

inline bool value::is_boolean() const noexcept
{
    return _type == value_type::boolean;
}

inline bool value::is_string() const noexcept
{
    return _type == value_type::string;
}

inline bool value::is_array() const noexcept
{
    return _type == value_type::array;
}

inline bool value::is_object() const noexcept
{
    return _type == value_type::object;
}

template <typename value_t>
inline bool value::is() const noexcept
{
    if constexpr (std::is_same_v<value, value_t>) {
        return true;
    }
    else if constexpr (_utils::has_check_json_in_member<value_t>::value) {
        value_t temp {};
        return temp.check_json(*this);
    }
    else if constexpr (_utils::has_check_json_in_templ_spec<value_t>::value) {
        return ext::jsonization<value_t>().check_json(*this);
    }
    else if constexpr (std::is_same_v<bool, value_t>) {
        return is_boolean();
    }
    else if constexpr (std::is_same_v<std::monostate, value_t>) {
        return is_null();
    }
    else if constexpr (std::is_same_v<std::nullptr_t, value_t>) {
        return is_null();
    }
    else if constexpr (_utils::is_nullable<value_t>) {
        return is_null() || is<_utils::nullable_value_t<value_t>>();
    }
    else if constexpr (std::is_enum_v<value_t>) {
        if (is_string()) {
            return _reflection::string_to_enum<value_t>(as_string_view()).has_value();
        }
        return is_number();
    }
    else if constexpr (std::is_arithmetic_v<value_t>) {
        return is_number();
    }
    else if constexpr (std::is_constructible_v<std::string, value_t>) {
        return is_string();
    }
#ifdef MEOJSON_FS_PATH_EXTENSION
    else if constexpr (std::is_same_v<std::decay_t<value_t>, std::filesystem::path>) {
        return is_string();
    }
#endif
    else if constexpr (std::is_same_v<array, value_t>) {
        return is_array();
    }
    else if constexpr (_utils::is_collection<value_t>) {
        return is_array() && all<typename value_t::value_type>();
    }
    else if constexpr (_utils::is_fixed_array<value_t>) {
        return is_array() && as_array().size() == _utils::fixed_array_size<value_t> && all<typename value_t::value_type>();
    }
    else if constexpr (_utils::is_tuple_like<value_t>) {
        if (!is_array()) {
            return false;
        }
        if (as_array().size() != std::tuple_size_v<value_t>) {
            return false;
        }
        return is_tuple_helper<value_t>(std::make_index_sequence<std::tuple_size_v<value_t>>());
    }
    else if constexpr (_utils::is_variant<value_t>) {
        return is_variant_helper(static_cast<value_t*>(nullptr));
    }
    else if constexpr (std::is_same_v<object, value_t>) {
        return is_object();
    }
    else if constexpr (_utils::is_map<value_t>) {
        return is_object() && std::is_constructible_v<std::string, typename value_t::key_type> && all<typename value_t::mapped_type>();
    }
    else {
        static_assert(!sizeof(value_t), "Unsupported type");
    }
}

inline bool value::contains(const std::string& key) const
{
    return is_object() && as_object().contains(key);
}

inline bool value::contains(size_t pos) const
{
    return is_array() && as_array().contains(pos);
}

inline bool value::exists(const std::string& key) const
{
    return contains(key);
}

inline bool value::exists(size_t pos) const
{
    return contains(pos);
}

inline value::value_type value::type() const noexcept
{
    return _type;
}

inline const value& value::at(size_t pos) const
{
    return as_array().at(pos);
}

inline const value& value::at(const std::string& key) const
{
    return as_object().at(key);
}

inline bool value::erase(size_t pos)
{
    return as_array().erase(pos);
}

inline bool value::erase(const std::string& key)
{
    return as_object().erase(key);
}

template <typename... key_then_default_value_t>
inline auto value::get(key_then_default_value_t&&... keys_then_default_value) const
{
    return get(std::forward_as_tuple(keys_then_default_value...), std::make_index_sequence<sizeof...(keys_then_default_value) - 1> {});
}

template <typename... key_then_default_value_t, size_t... keys_indexes_t>
inline auto value::get(std::tuple<key_then_default_value_t...> keys_then_default_value, std::index_sequence<keys_indexes_t...>) const
{
    constexpr unsigned long default_value_index = sizeof...(key_then_default_value_t) - 1;
    return get_helper(std::get<default_value_index>(keys_then_default_value), std::get<keys_indexes_t>(keys_then_default_value)...);
}

template <typename value_t, typename first_key_t, typename... rest_keys_t>
inline auto value::get_helper(const value_t& default_value, first_key_t&& first, rest_keys_t&&... rest) const
{
    if constexpr (std::is_constructible_v<std::string, first_key_t>) {
        return is_object() ? as_object().get_helper(default_value, std::forward<first_key_t>(first), std::forward<rest_keys_t>(rest)...)
                           : default_value;
    }
    else if constexpr (std::is_integral_v<std::decay_t<first_key_t>>) {
        return is_array() ? as_array().get_helper(default_value, std::forward<first_key_t>(first), std::forward<rest_keys_t>(rest)...)
                          : default_value;
    }
    else {
        static_assert(!sizeof(first_key_t), "Parameter must be integral or std::string constructible");
    }
}

template <typename value_t, typename unique_key_t>
inline auto value::get_helper(const value_t& default_value, unique_key_t&& first) const
{
    if constexpr (std::is_constructible_v<std::string, unique_key_t>) {
        return is_object() ? as_object().get_helper(default_value, std::forward<unique_key_t>(first)) : default_value;
    }
    else if constexpr (std::is_integral_v<std::decay_t<unique_key_t>>) {
        return is_array() ? as_array().get_helper(default_value, std::forward<unique_key_t>(first)) : default_value;
    }
    else {
        static_assert(!sizeof(unique_key_t), "Parameter must be integral or std::string constructible");
    }
}

template <typename value_t>
inline std::optional<value_t> value::find(size_t pos) const
{
    return is_array() ? as_array().template find<value_t>(pos) : std::nullopt;
}

template <typename value_t>
inline std::optional<value_t> value::find(const std::string& key) const
{
    return is_object() ? as_object().template find<value_t>(key) : std::nullopt;
}

inline bool value::as_boolean() const
{
    if (is_boolean()) {
        if (const std::string& b_str = as_basic_type_str(); b_str == _utils::true_string()) {
            return true;
        }
        else if (b_str == _utils::false_string()) {
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

inline int value::as_integer() const
{
    if (is_number()) {
        return std::stoi(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline unsigned value::as_unsigned() const
{
    // I don't know why there is no std::stou.
    return static_cast<unsigned>(as_unsigned_long());
}

inline long value::as_long() const
{
    if (is_number()) {
        return std::stol(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline unsigned long value::as_unsigned_long() const
{
    if (is_number()) {
        return std::stoul(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline long long value::as_long_long() const
{
    if (is_number()) {
        return std::stoll(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline unsigned long long value::as_unsigned_long_long() const
{
    if (is_number()) {
        return std::stoull(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline float value::as_float() const
{
    if (is_number()) {
        return std::stof(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline double value::as_double() const
{
    if (is_number()) {
        return std::stod(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline long double value::as_long_double() const
{
    if (is_number()) {
        return std::stold(as_basic_type_str());
    }
    else {
        throw exception("Wrong Type");
    }
}

inline std::string value::as_string() const
{
    if (is_string()) {
        return as_basic_type_str();
    }
    else {
        throw exception("Wrong Type");
    }
}

inline std::string_view value::as_string_view() const
{
    if (is_string()) {
        return as_basic_type_str();
    }
    else {
        throw exception("Wrong Type");
    }
}

inline const array& value::as_array() const
{
    if (is_array()) {
        return *std::get<array_ptr>(_raw_data);
    }

    throw exception("Wrong Type");
}

inline const object& value::as_object() const
{
    if (is_object()) {
        return *std::get<object_ptr>(_raw_data);
    }

    throw exception("Wrong Type or data empty");
}

inline array& value::as_array()
{
    if (empty()) {
        _type = value_type::array;
        _raw_data = std::make_unique<array>();
    }

    if (is_array()) {
        return *std::get<array_ptr>(_raw_data);
    }

    throw exception("Wrong Type");
}

inline object& value::as_object()
{
    if (empty()) {
        _type = value_type::object;
        _raw_data = std::make_unique<object>();
    }

    if (is_object()) {
        return *std::get<object_ptr>(_raw_data);
    }

    throw exception("Wrong Type or data empty");
}

template <typename tuple_t, size_t... Is>
inline bool value::is_tuple_helper(std::index_sequence<Is...>) const noexcept
{
    const auto& arr = as_array();
    return (arr[Is].is<std::tuple_element_t<Is, tuple_t>>() && ...);
}

template <typename... Ts>
inline bool value::is_variant_helper(std::variant<Ts...>*) const noexcept
{
    return (is<Ts>() || ...);
}

template <typename value_t>
inline value_t value::as() const&
{
    if constexpr (std::is_same_v<value, value_t>) {
        return *this;
    }
    else if constexpr (_utils::has_from_json_in_member<value_t>::value) {
        value_t dst {};
        if (!dst.from_json(*this)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }
    else if constexpr (_utils::has_from_json_in_templ_spec<value_t>::value) {
        value_t dst {};
        if (!ext::jsonization<value_t>().from_json(*this, dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }
    else {
        return static_cast<value_t>(*this);
    }
}

template <typename value_t>
inline value_t value::as() &&
{
    if constexpr (std::is_same_v<value, value_t>) {
        return std::move(*this);
    }
    else if constexpr (_utils::has_from_json_in_member<value_t>::value) {
        value_t dst {};
        if (!dst.from_json(*this)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }
    else if constexpr (_utils::has_move_from_json_in_templ_spec<value_t>::value) {
        value_t dst {};
        if (!ext::jsonization<value_t>().move_from_json(std::move(*this), dst)) {
            throw exception("Wrong JSON");
        }
        return dst;
    }
    else {
        return static_cast<value_t>(*this);
    }
}

inline const std::string& value::as_basic_type_str() const
{
    return std::get<std::string>(_raw_data);
}

inline std::string& value::as_basic_type_str()
{
    return std::get<std::string>(_raw_data);
}

template <typename... args_t>
inline decltype(auto) value::emplace(args_t&&... args)
{
    constexpr bool is_array_args = std::is_constructible_v<typename array::value_type, args_t...>;
    constexpr bool is_object_args = std::is_constructible_v<typename object::value_type, args_t...>;

    static_assert(is_array_args || is_object_args, "Args can not constructure a array or object value");

    if constexpr (is_array_args) {
        return as_array().emplace_back(std::forward<args_t>(args)...);
    }
    else if constexpr (is_object_args) {
        return as_object().emplace(std::forward<args_t>(args)...);
    }
}

inline void value::clear() noexcept
{
    *this = value();
}

inline std::string value::to_string() const
{
    switch (_type) {
    case value_type::null:
        return std::string(_utils::null_string());
    case value_type::boolean:
    case value_type::number:
        return as_basic_type_str();
    case value_type::string:
        return '"' + _utils::unescape_string(as_basic_type_str()) + '"';
    case value_type::array:
        return as_array().to_string();
    case value_type::object:
        return as_object().to_string();
    default:
        throw exception("Unknown value Type");
    }
}

inline std::string value::format(size_t indent, size_t indent_times) const
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
        throw exception("Unknown value Type");
    }
}

inline std::string value::dumps(std::optional<size_t> indent) const
{
    return indent ? format(*indent) : to_string();
}

inline std::string value::format(size_t indent) const
{
    return format(indent, 0);
}

template <typename value_t>
inline bool value::all() const
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

inline value& value::operator=(const value& rhs)
{
    _type = rhs._type;
    _raw_data = deep_copy(rhs._raw_data);

    return *this;
}

inline value& value::operator=(value&& rhs) noexcept = default;

inline bool value::operator==(const value& rhs) const
{
    if (_type != rhs._type) {
        return false;
    }

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
        throw exception("Unknown value Type");
    }
}

inline bool value::operator!=(const value& rhs) const
{
    return !(*this == rhs);
}

inline value::operator bool() const
{
    return as_boolean();
}

inline value::operator int() const
{
    return as_integer();
}

inline value::operator unsigned() const
{
    return as_unsigned();
}

inline value::operator long() const
{
    return as_long();
}

inline value::operator unsigned long() const
{
    return as_unsigned_long();
}

inline value::operator long long() const
{
    return as_long_long();
}

inline value::operator unsigned long long() const
{
    return as_unsigned_long_long();
}

inline value::operator float() const
{
    return as_float();
}

inline value::operator double() const
{
    return as_double();
}

inline value::operator long double() const
{
    return as_long_double();
}

inline value::operator std::string() const
{
    return as_string();
}

inline value::operator std::string_view() const&
{
    return as_string_view();
}

inline value::operator array() const
{
    return as_array();
}

inline value::operator object() const
{
    return as_object();
}

inline value::operator std::nullptr_t() const
{
    if (is_null()) {
        return nullptr;
    }
    else {
        throw exception("Wrong Type");
    }
}

inline value::operator std::monostate() const
{
    if (is_null()) {
        return std::monostate {};
    }
    else {
        throw exception("Wrong Type");
    }
}

inline const value& value::operator[](size_t pos) const
{
    // array not support to create by operator[]

    return as_array()[pos];
}

inline value& value::operator[](size_t pos)
{
    // array not support to create by operator[]

    return as_array()[pos];
}

inline value& value::operator[](const std::string& key)
{
    if (empty()) {
        _type = value_type::object;
        _raw_data = std::make_unique<object>();
    }

    return as_object()[key];
}

inline value& value::operator[](std::string&& key)
{
    if (empty()) {
        _type = value_type::object;
        _raw_data = std::make_unique<object>();
    }

    return as_object()[std::move(key)];
}

inline value value::operator|(const object& rhs) const&
{
    value result;
    result._type = value_type::object;
    result._raw_data = std::make_unique<object>(as_object() | rhs);
    return result;
}

inline value value::operator|(object&& rhs) const&
{
    value result;
    result._type = value_type::object;
    result._raw_data = std::make_unique<object>(as_object() | std::move(rhs));
    return result;
}

inline value value::operator|(const object& rhs) &&
{
    value result;
    result._type = value_type::object;
    result._raw_data = std::make_unique<object>(std::move(as_object()) | rhs);
    return result;
}

inline value value::operator|(object&& rhs) &&
{
    value result;
    result._type = value_type::object;
    result._raw_data = std::make_unique<object>(std::move(as_object()) | std::move(rhs));
    return result;
}

inline value& value::operator|=(const object& rhs)
{
    as_object() |= rhs;
    return *this;
}

inline value& value::operator|=(object&& rhs)
{
    as_object() |= std::move(rhs);
    return *this;
}

inline value value::operator+(const array& rhs) const&
{
    value result;
    result._type = value_type::array;
    result._raw_data = std::make_unique<array>(as_array() + rhs);
    return result;
}

inline value value::operator+(array&& rhs) const&
{
    value result;
    result._type = value_type::array;
    result._raw_data = std::make_unique<array>(as_array() + std::move(rhs));
    return result;
}

inline value value::operator+(const array& rhs) &&
{
    value result;
    result._type = value_type::array;
    result._raw_data = std::make_unique<array>(std::move(as_array()) + rhs);
    return result;
}

inline value value::operator+(array&& rhs) &&
{
    value result;
    result._type = value_type::array;
    result._raw_data = std::make_unique<array>(std::move(as_array()) + std::move(rhs));
    return result;
}

inline value& value::operator+=(const array& rhs)
{
    as_array() += rhs;
    return *this;
}

inline value& value::operator+=(array&& rhs)
{
    as_array() += std::move(rhs);
    return *this;
}

template <typename... args_t>
inline value::value(value_type type, args_t&&... args)
    : _type(type)
    , _raw_data(std::forward<args_t>(args)...)
{
    static_assert(std::is_constructible_v<var_t, args_t...>, "Parameter can't be used to construct a var_t");
}

inline typename value::var_t value::deep_copy(const var_t& src)
{
    var_t dst;
    if (const auto string_ptr = std::get_if<std::string>(&src)) {
        dst = *string_ptr;
    }
    else if (const auto arr_ptr = std::get_if<array_ptr>(&src)) {
        dst = std::make_unique<array>(**arr_ptr);
    }
    else if (const auto obj_ptr = std::get_if<object_ptr>(&src)) {
        dst = std::make_unique<object>(**obj_ptr);
    }
    else {
        // maybe invalid_value
    }

    return dst;
}

inline std::ostream& operator<<(std::ostream& out, const value& val)
{
    out << val.format();
    return out;
}

// Implementation of variant constructors
template <
    typename... Ts,
    std::enable_if_t<
        (sizeof...(Ts) > 0) && !_utils::has_to_json_in_member<std::variant<Ts...>>::value
            && !_utils::has_to_json_in_templ_spec<std::variant<Ts...>>::value,
        bool>>
inline value::value(const std::variant<Ts...>& var)
{
    std::visit([this](const auto& val) { *this = value(val); }, var);
}

template <
    typename... Ts,
    std::enable_if_t<
        (sizeof...(Ts) > 0) && !_utils::has_to_json_in_member<std::variant<Ts...>>::value
            && !_utils::has_to_json_in_templ_spec<std::variant<Ts...>>::value,
        bool>>
inline value::value(std::variant<Ts...>&& var)
{
    std::visit([this](auto&& val) { *this = value(std::move(val)); }, std::move(var));
}

template <typename... Ts>
inline std::variant<Ts...> value::to_variant_helper() const&
{
    std::variant<Ts...> result;
    if (!((is<Ts>() ? (result = as<Ts>(), true) : false) || ...)) {
        throw exception("Cannot convert JSON value to any variant alternative");
    }
    return result;
}

template <typename... Ts>
inline std::variant<Ts...> value::move_to_variant_helper() &&
{
    std::variant<Ts...> result;
    if (!((is<Ts>() ? (result = std::move(*this).as<Ts>(), true) : false) || ...)) {
        throw exception("Cannot convert JSON value to any variant alternative");
    }
    return result;
}

// Implementation of variant conversion operators
template <typename... Ts>
inline value::operator std::variant<Ts...>() const&
{
    return to_variant_helper<Ts...>();
}

template <typename... Ts>
inline value::operator std::variant<Ts...>() &&
{
    return std::move(*this).move_to_variant_helper<Ts...>();
}
} // namespace json
