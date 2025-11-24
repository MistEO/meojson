#pragma once

#include <array>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

#ifndef MEOJSON_ENUM_REFLECTION_MIN_ENUMS
#define MEOJSON_ENUM_REFLECTION_MIN_ENUMS -128
#endif
#ifndef MEOJSON_ENUM_REFLECTION_MAX_ENUMS
#define MEOJSON_ENUM_REFLECTION_MAX_ENUMS 128
#endif

namespace json
{
namespace _reflection
{
template <typename E, E V>
constexpr std::string_view name() noexcept
{
#if defined(__clang__) || defined(__GNUG__)
    std::string_view name = __PRETTY_FUNCTION__;
    auto start = name.find("V = ");
    if (start == std::string_view::npos) {
        return {};
    }
    start += 4;
    auto end = name.find_last_of(']');
    if (end == std::string_view::npos) {
        return {};
    }
    name = name.substr(start, end - start);
    auto sem = name.find(';');
    if (sem != std::string_view::npos) {
        name = name.substr(0, sem);
    }
    auto sep = name.find_last_of(':');
    if (sep != std::string_view::npos) {
        name = name.substr(sep + 1);
    }
    return name;
#elif defined(_MSC_VER)
    std::string_view name = __FUNCSIG__;
    auto start = name.find(',');
    if (start == std::string_view::npos) {
        return {};
    }
    start += 1;
    while (start < name.size() && name[start] == ' ') {
        start++;
    }

    auto end = name.find_last_of('>');
    if (end == std::string_view::npos) {
        return {};
    }

    name = name.substr(start, end - start);

    auto sep = name.find_last_of(':');
    if (sep != std::string_view::npos) {
        name = name.substr(sep + 1);
    }
    return name;
#else
    return {};
#endif
}

template <typename E, E V>
constexpr bool is_valid() noexcept
{
    std::string_view n = name<E, V>();
    if (n.empty()) {
        return false;
    }

    // 更严格的验证：检查是否为有效的C++标识符
    // 第一个字符必须是字母或下划线
    char first_char = n[0];
    if (!((first_char >= 'a' && first_char <= 'z') || (first_char >= 'A' && first_char <= 'Z') || first_char == '_')) {
        return false;
    }

    // 后续字符可以是字母、数字或下划线
    for (size_t i = 1; i < n.size(); ++i) {
        char c = n[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
            return false;
        }
    }

    // 额外的检查：排除编译器生成的名称
    // 这些通常是数字或以特殊字符开头
    if (n.find('(') != std::string_view::npos || n.find(')') != std::string_view::npos || n.find('{') != std::string_view::npos
        || n.find('}') != std::string_view::npos) {
        return false;
    }

    return true;
}

template <typename E>
struct enum_range
{
    static constexpr int min = MEOJSON_ENUM_REFLECTION_MIN_ENUMS;
    static constexpr int max = MEOJSON_ENUM_REFLECTION_MAX_ENUMS;
};

template <typename E, int... Is>
constexpr auto get_enum_names(std::integer_sequence<int, Is...>)
{
    constexpr int min = enum_range<E>::min;
    return std::array<std::string_view, sizeof...(Is)> { { (
        is_valid<E, static_cast<E>(Is + min)>() ? name<E, static_cast<E>(Is + min)>() : std::string_view {})... } };
}

// Static storage for enum names to avoid C++23 extension warning
template <typename E>
struct enum_name_storage
{
    static constexpr int min = enum_range<E>::min;
    static constexpr int max = enum_range<E>::max;
    static constexpr int range = max - min + 1;
    static constexpr auto names = get_enum_names<E>(std::make_integer_sequence<int, range> {});
};

template <typename E>
constexpr std::string_view enum_to_string(E val)
{
    constexpr int min = enum_range<E>::min;
    constexpr int max = enum_range<E>::max;
    constexpr int range = max - min + 1;

    const auto& names = enum_name_storage<E>::names;

    int idx = static_cast<int>(val) - min;
    if (idx >= 0 && idx < range) {
        return names[idx];
    }
    return {};
}

constexpr bool iequals(std::string_view a, std::string_view b)
{
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        char c1 = a[i];
        char c2 = b[i];
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 += 32;
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 32;
        }
        if (c1 != c2) {
            return false;
        }
    }
    return true;
}

template <typename E>
std::optional<E> string_to_enum(std::string_view str)
{
    constexpr int min = enum_range<E>::min;
    constexpr int range = enum_name_storage<E>::range;

    const auto& names = enum_name_storage<E>::names;

    for (int i = 0; i < range; ++i) {
        if (!names[i].empty()) {
            if (iequals(names[i], str)) {
                return static_cast<E>(i + min);
            }
        }
    }
    return std::nullopt;
}
} // namespace _reflection
} // namespace json
