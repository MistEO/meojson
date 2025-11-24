// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

namespace json
{
class value;
class array;
class object;
}

namespace json::ext
{
template <typename T, typename = void>
class jsonization
{
public:
    // value to_json(const T&) const;
    // bool check_json(const value&) const;
    // bool from_json(const value&, T&) const;
};
}

namespace json::_utils
{
template <typename T>
using iterator_t = decltype(std::declval<T&>().begin());
template <typename T>
using iter_value_t = typename std::iterator_traits<std::decay_t<T>>::value_type;
template <typename R>
using range_value_t = iter_value_t<iterator_t<R>>;

template <typename T, typename = void>
constexpr bool is_string = false;
template <typename T>
constexpr bool is_string<T, std::void_t<typename T::traits_type>> =
    std::is_same_v<typename T::traits_type, std::char_traits<typename T::value_type>>;

template <typename T, typename = void>
constexpr bool is_container = false;
template <typename T>
constexpr bool is_container<T, std::void_t<typename T::value_type, range_value_t<T>>> =
    std::is_same_v<typename T::value_type, range_value_t<T>> && !is_string<T>;

template <typename T, typename = void>
constexpr bool is_map = false;
template <typename T>
constexpr bool is_map<T, std::void_t<typename T::key_type, typename T::mapped_type>> = is_container<T>;

template <typename T, typename = void>
constexpr bool is_fixed_array = false;
template <template <typename, size_t> typename arr_t, typename value_t, size_t size>
constexpr bool is_fixed_array<arr_t<value_t, size>> = true;

template <typename T, typename = std::enable_if_t<is_fixed_array<T>>>
constexpr size_t fixed_array_size = 0;
template <template <typename, size_t> typename arr_t, typename value_t, size_t size>
constexpr size_t fixed_array_size<arr_t<value_t, size>> = size;

template <typename T, typename = void>
constexpr bool is_collection = false;
template <typename T>
constexpr bool is_collection<T> = is_container<T> && !is_map<T> && !is_fixed_array<T>;

template <typename T, typename = void>
constexpr bool is_nullable = false;
template <typename T>
constexpr bool is_nullable<
    T,
    std::void_t<
        decltype(static_cast<bool>(std::declval<T>())), // 检查 operator bool
        decltype(*std::declval<T>())                    // 检查 operator*
        >> = !std::is_pointer_v<std::decay_t<T>>;       // 排除原生指针

template <typename T, typename = std::enable_if_t<is_nullable<T>>>
using nullable_value_t = std::decay_t<decltype(*std::declval<T>())>;

template <typename T>
constexpr bool is_variant = false;
template <typename... args_t>
constexpr bool is_variant<std::variant<args_t...>> = true;

template <typename T>
constexpr bool is_tuple = false;
template <typename... args_t>
constexpr bool is_tuple<std::tuple<args_t...>> = true;

template <typename T>
constexpr bool is_pair = false;
template <typename... args_t>
constexpr bool is_pair<std::pair<args_t...>> = true;

template <typename T, typename = void>
constexpr bool is_tuple_like = false;
template <template <typename...> typename tuple_t, typename... args_t>
constexpr bool is_tuple_like<tuple_t<args_t...>, std::void_t<decltype(std::tuple_size<tuple_t<args_t...>>::value)>> =
    std::tuple_size<tuple_t<args_t...>>::value == sizeof...(args_t);

template <typename T, typename = void>
struct has_emplace_back : std::false_type
{
};

template <typename T>
struct has_emplace_back<T, std::void_t<decltype(std::declval<T>().emplace_back())>> : std::true_type
{
};

template <typename T, typename = void>
struct has_to_json_in_member : std::false_type
{
};

template <typename T>
struct has_to_json_in_member<T, std::void_t<decltype(std::declval<T>().to_json())>> : std::true_type
{
};

template <typename T, typename = void>
struct has_check_json_in_member : std::false_type
{
};

template <typename T>
struct has_check_json_in_member<T, std::void_t<decltype(std::declval<T>().check_json(std::declval<value>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_from_json_in_member : std::false_type
{
};

template <typename T>
struct has_from_json_in_member<T, std::void_t<decltype(std::declval<T>().from_json(std::declval<value>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_to_json_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_to_json_in_templ_spec<T, std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().to_json(std::declval<T>()))>>
    : std::true_type
{
};

template <typename T, typename = void>
struct has_check_json_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_check_json_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().check_json(std::declval<value>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_from_json_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_from_json_in_templ_spec<
    T,
    std::void_t<
        decltype(std::declval<ext::jsonization<std::decay_t<T>>>().from_json(std::declval<value>(), std::declval<std::decay_t<T>&>()))>>
    : std::true_type
{
};

template <typename T, typename = void>
struct has_move_to_json_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_move_to_json_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().move_to_json(std::declval<T>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_move_from_json_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_move_from_json_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>()
                             .move_from_json(std::declval<value>(), std::declval<std::decay_t<T>&>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_to_json_array_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_to_json_array_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().to_json_array(std::declval<T>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_check_json_array_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_check_json_array_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().check_json_array(std::declval<array>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_from_json_array_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_from_json_array_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>()
                             .from_json_array(std::declval<array>(), std::declval<std::decay_t<T>&>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_move_to_json_array_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_move_to_json_array_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().move_to_json_array(std::declval<T>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_move_from_json_array_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_move_from_json_array_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>()
                             .move_from_json_array(std::declval<array>(), std::declval<std::decay_t<T>&>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_to_json_object_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_to_json_object_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().to_json_object(std::declval<T>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_check_json_object_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_check_json_object_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().check_json_object(std::declval<object>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_from_json_object_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_from_json_object_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>()
                             .from_json_object(std::declval<object>(), std::declval<std::decay_t<T>&>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_move_to_json_object_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_move_to_json_object_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>().move_to_json_object(std::declval<T>()))>> : std::true_type
{
};

template <typename T, typename = void>
struct has_move_from_json_object_in_templ_spec : std::false_type
{
};

template <typename T>
struct has_move_from_json_object_in_templ_spec<
    T,
    std::void_t<decltype(std::declval<ext::jsonization<std::decay_t<T>>>()
                             .move_from_json_object(std::declval<object>(), std::declval<std::decay_t<T>&>()))>> : std::true_type
{
};

inline std::string unescape_string(const std::string& str)
{
    std::string result;
    auto cur = str.cbegin();
    auto end = str.cend();
    auto no_escape_beg = cur;
    char escape = 0;

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
            result += std::string(no_escape_beg, cur) + '\\' + escape;
            no_escape_beg = cur + 1;
            escape = 0;
        }
    }
    result += std::string(no_escape_beg, cur);

    return result;
}

inline std::string_view true_string()
{
    return "true";
}

inline std::string_view false_string()
{
    return "false";
}

inline std::string_view null_string()
{
    return "null";
}

template <typename any_t>
inline std::string to_basic_string(any_t&& arg)
{
#ifdef MEOJSON_KEEP_FLOATING_PRECISION
    using real_type = std::remove_reference_t<any_t>;
    if constexpr (std::is_floating_point_v<real_type>) {
        std::ostringstream oss;
        oss << std::setprecision(std::numeric_limits<real_type>::max_digits10) << arg;
        return oss.str();
    }
    else
#endif
    {
        return std::to_string(std::forward<any_t>(arg));
    }
}
} // namespace json::_utils
