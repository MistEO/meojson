#pragma once

#include <string>

#include "constexpr_map.hpp"
#include "constexpr_unique_ptr.hpp"

namespace json
{

template <typename elem_t>
constexpr bool soft_is_digit(elem_t ch)
{
    return '0' <= ch && ch <= '9';
}

template <typename string_t>
struct soft_to_string_t
{

    template <typename any_t>
    constexpr string_t to_string(any_t v) = delete;

    template <>
    constexpr string_t to_string<unsigned long long>(unsigned long long value)
    {
        string_t result;
        do {
            result.push_back('0' + value % 10);
            value /= 10;
        } while (value > 0);
        return string_t(result.rbegin(), result.rend());
    }

    template <>
    constexpr string_t to_string<long long>(long long value)
    {
        string_t result;
        bool nega = false;
        if (value == INT64_MIN) {
            return to_string<unsigned long long>(0x8000000000000000ull);
        }
        if (value < 0) {
            nega = true;
            value = -value;
        }
        do {
            result.push_back('0' + value % 10);
            value /= 10;
        } while (value > 0);
        if (nega) {
            result.push_back('-');
        }
        return string_t(result.rbegin(), result.rend());
    }

    template <>
    constexpr string_t to_string<double>(double value)
    {
        string_t result;
        if (value < 0) {
            result.push_back('-');
            value = -value;
        }
        int precision = 14;
        double base = 1;
        while (base < value) {
            base *= 10;
        }
        while (precision--) {
            base /= 10;
            char c = '0';
            while (value >= base) {
                value -= base;
                c++;
            }
            result.push_back(c);
            if (base == 1) {
                result.push_back('.');
            }
        }
        return result;
    }

    template <>
    constexpr string_t to_string<signed char>(signed char v)
    {
        return to_string<long long>(v);
    }
    template <>
    constexpr string_t to_string<short>(short v)
    {
        return to_string<long long>(v);
    }
    template <>
    constexpr string_t to_string<int>(int v)
    {
        return to_string<long long>(v);
    }
    template <>
    constexpr string_t to_string<long>(long v)
    {
        return to_string<long long>(v);
    }

    template <>
    constexpr string_t to_string<unsigned char>(unsigned char v)
    {
        return to_string<unsigned long long>(v);
    }
    template <>
    constexpr string_t to_string<unsigned short>(unsigned short v)
    {
        return to_string<unsigned long long>(v);
    }
    template <>
    constexpr string_t to_string<unsigned int>(unsigned int v)
    {
        return to_string<unsigned long long>(v);
    }
    template <>
    constexpr string_t to_string<unsigned long>(unsigned long v)
    {
        return to_string<unsigned long long>(v);
    }

    template <>
    constexpr string_t to_string<float>(float v)
    {
        return to_string<double>(v);
    }
    template <>
    constexpr string_t to_string<long double>(long double v)
    {
        return to_string<double>(static_cast<double>(v));
    }
};

} // namespace json