#pragma once

#include "parser.hpp"

namespace json
{
// *************************
// *      parser impl      *
// *************************

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline std::optional<value> parser<accept_jsonc, parsing_t, accel_traits>::parse(const parsing_t& content)
{
    return parser<accept_jsonc, parsing_t, accel_traits>(content.cbegin(), content.cend()).parse();
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline std::optional<value> parser<accept_jsonc, parsing_t, accel_traits>::parse()
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
    default: // A JSON payload should be an object or array
        return std::nullopt;
    }

    if (!result_value.valid()) {
        return std::nullopt;
    }

    // After the parsing is complete, there should be no more content other than spaces behind
    if (skip_whitespace()) {
        return std::nullopt;
    }

    return result_value;
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_value()
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

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_null()
{
    for (const auto& ch : _utils::null_string()) {
        if (_cur != _end && *_cur == ch) {
            ++_cur;
        }
        else {
            return invalid_value();
        }
    }

    return value();
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_boolean()
{
    switch (*_cur) {
    case 't':
        for (const auto& ch : _utils::true_string()) {
            if (_cur != _end && *_cur == ch) {
                ++_cur;
            }
            else {
                return invalid_value();
            }
        }
        return true;
    case 'f':
        for (const auto& ch : _utils::false_string()) {
            if (_cur != _end && *_cur == ch) {
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

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_number()
{
    const auto first = _cur;
    if (*_cur == '-') {
        ++_cur;
    }

    // numbers cannot have leading zeroes
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

    return value(value::value_type::number, std::string(first, _cur));
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_string()
{
    auto string_opt = parse_stdstring();
    if (!string_opt) {
        return invalid_value();
    }
    return value(value::value_type::string, std::move(string_opt).value());
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_array()
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

    typename array::raw_array result;
    while (true) {
        if (!skip_whitespace()) {
            return invalid_value();
        }

        if constexpr (accept_jsonc) {
            if (*_cur == ']') {
                break;
            }
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

    return array(std::move(result));
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline value parser<accept_jsonc, parsing_t, accel_traits>::parse_object()
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

    typename object::raw_object result;
    while (true) {
        if (!skip_whitespace()) {
            return invalid_value();
        }

        if constexpr (accept_jsonc) {
            if (*_cur == '}') {
                break;
            }
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

        auto emplaced = result.emplace(std::move(*key_opt), std::move(val)).second;
        if (!emplaced) {
            return invalid_value();
        }

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

    return object(std::move(result));
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline std::optional<std::string> parser<accept_jsonc, parsing_t, accel_traits>::parse_stdstring()
{
    if (*_cur == '"') {
        ++_cur;
    }
    else {
        return std::nullopt;
    }

    std::string result;
    auto no_escape_beg = _cur;
    uint16_t pair_high = 0;

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
            result.append(no_escape_beg, _cur++);
            if (_cur == _end) {
                return std::nullopt;
            }
            if (pair_high && *_cur != 'u') {
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
            case 'u':
                if (!skip_unicode_escape(pair_high, result)) {
                    return std::nullopt;
                }
                break;
            default:
                // Illegal backslash escape
                return std::nullopt;
            }
            no_escape_beg = ++_cur;
            break;
        }
        case '"': {
            if (pair_high) {
                return std::nullopt;
            }
            result.append(no_escape_beg, _cur++);
            return result;
        }
        default:
            if (pair_high) {
                return std::nullopt;
            }
            ++_cur;
            break;
        }
    }
    return std::nullopt;
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline bool parser<accept_jsonc, parsing_t, accel_traits>::skip_unicode_escape(uint16_t& pair_high, std::string& result)
{
    uint16_t cp = 0;
    for (int i = 0; i < 4; ++i) {
        if (++_cur == _end) {
            return false;
        }

        if (!std::isxdigit(static_cast<unsigned char>(*_cur))) {
            return false;
        }

        cp <<= 4;

        if ('0' <= *_cur && *_cur <= '9') {
            cp |= *_cur - '0';
        }
        else if ('a' <= *_cur && *_cur <= 'f') {
            cp |= *_cur - 'a' + 10;
        }
        else if ('A' <= *_cur && *_cur <= 'F') {
            cp |= *_cur - 'A' + 10;
        }
        else {
            return false;
        }
    }

    uint32_t ext_cp = cp;

    if (0xD800 <= cp && cp <= 0xDBFF) {
        if (pair_high) {
            return false;
        }
        pair_high = cp;
        return true;
    }

    if (0xDC00 <= cp && cp <= 0xDFFF) {
        if (!pair_high) {
            return false;
        }
        ext_cp = (((pair_high - 0xD800) << 10) | (cp - 0xDC00)) + 0x10000;
        pair_high = 0;
    }

    // utf8
    if (ext_cp <= 0x7F) {
        result.push_back(static_cast<char>(ext_cp));
    }
    else if (ext_cp <= 0x7FF) {
        result.push_back(static_cast<char>(((ext_cp >> 6) & 0b00011111) | 0b11000000u));
        result.push_back(static_cast<char>((ext_cp & 0b00111111) | 0b10000000u));
    }
    else if (ext_cp <= 0xFFFF) {
        result.push_back(static_cast<char>(((ext_cp >> 12) & 0b00001111) | 0b11100000u));
        result.push_back(static_cast<char>(((ext_cp >> 6) & 0b00111111) | 0b10000000u));
        result.push_back(static_cast<char>((ext_cp & 0b00111111) | 0b10000000u));
    }
    else {
        result.push_back(static_cast<char>(((ext_cp >> 18) & 0b00000111) | 0b11110000u));
        result.push_back(static_cast<char>(((ext_cp >> 12) & 0b00111111) | 0b10000000u));
        result.push_back(static_cast<char>(((ext_cp >> 6) & 0b00111111) | 0b10000000u));
        result.push_back(static_cast<char>((ext_cp & 0b00111111) | 0b10000000u));
    }

    return true;
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline bool parser<accept_jsonc, parsing_t, accel_traits>::skip_string_literal_with_accel()
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

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline bool parser<accept_jsonc, parsing_t, accel_traits>::skip_whitespace() noexcept
{
    while (_cur != _end) {
        switch (*_cur) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            ++_cur;
            break;
        case '/':
            if constexpr (accept_jsonc) {
                if (!skip_comment()) {
                    return false;
                }
                // else continue;
            }
            else {
                return false;
            }
            break;
        case '\0':
            return false;
        default:
            return true;
        }
    }
    return false;
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline bool parser<accept_jsonc, parsing_t, accel_traits>::skip_comment() noexcept
{
    if (_cur == _end || *_cur != '/') {
        return false;
    }

    if (++_cur == _end) {
        return false;
    }

    enum class comment_type
    {
        invalid,
        line,
        block,
    } t = comment_type::invalid;

    switch (*_cur++) {
    case '/':
        t = comment_type::line;
        break;
    case '*':
        t = comment_type::block;
        break;
    default:
        return false;
    }

    while (_cur != _end) {
        switch (*_cur++) {
        case '\n':
            if (t == comment_type::line) {
                return true;
            }
            break;
        case '*':
            if (t == comment_type::block && _cur != _end && *_cur == '/') {
                ++_cur;
                return true;
            }
            break;
        default:
            break;
        }
    }

    // _cur == _end
    return t == comment_type::line;
}

template <bool accept_jsonc, typename parsing_t, typename accel_traits>
inline bool parser<accept_jsonc, parsing_t, accel_traits>::skip_digit()
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
// *      utils impl       *
// *************************

template <typename parsing_t>
inline std::optional<value> parse(const parsing_t& content)
{
    return parser<false, parsing_t>::parse(content);
}

inline std::optional<value> parse(const char* content)
{
    return parse(std::string_view { content });
}

inline std::optional<value> parse(std::istream& istream, bool check_bom, bool with_commets)
{
    istream.seekg(0, std::ios::end);
    auto file_size = istream.tellg();

    istream.seekg(0, std::ios::beg);
    std::string str(file_size, '\0');

    istream.read(str.data(), file_size);

    if (check_bom) {
        using uchar = unsigned char;
        static constexpr uchar Bom_0 = 0xEF;
        static constexpr uchar Bom_1 = 0xBB;
        static constexpr uchar Bom_2 = 0xBF;

        if (str.size() >= 3 && static_cast<uchar>(str.at(0)) == Bom_0 && static_cast<uchar>(str.at(1)) == Bom_1
            && static_cast<uchar>(str.at(2)) == Bom_2) {
            str.assign(str.begin() + 3, str.end());
        }
    }
    return with_commets ? parsec(str) : parse(str);
}

template <typename path_t>
inline std::optional<value> open(const path_t& filepath, bool check_bom, bool with_commets)
{
    std::ifstream ifs(filepath, std::ios::in);
    if (!ifs.is_open()) {
        return std::nullopt;
    }
    auto opt = parse(ifs, check_bom, with_commets);
    ifs.close();
    return opt;
}

template <typename parsing_t>
inline std::optional<value> parsec(const parsing_t& content)
{
    return parser<true, parsing_t>::parse(content);
}

inline std::optional<value> parsec(const char* content)
{
    return parsec(std::string_view { content });
}

namespace literals
{
inline value operator""_json(const char* str, size_t len)
{
    return operator""_jvalue(str, len);
}

inline value operator""_jvalue(const char* str, size_t len)
{
    return parse(std::string_view(str, len)).value_or(value());
}

inline array operator""_jarray(const char* str, size_t len)
{
    auto val = parse(std::string_view(str, len)).value_or(value());
    return val.is_array() ? val.as_array() : array();
}

inline object operator""_jobject(const char* str, size_t len)
{
    auto val = parse(std::string_view(str, len)).value_or(value());
    return val.is_object() ? val.as_object() : object();
}
} // namespace literals

inline const value invalid_value()
{
    return value(value::value_type::invalid, typename value::var_t());
}
} // namespace json

