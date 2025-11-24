// IWYU pragma: private, include <meojson/json.hpp>

#pragma once

#include <cctype>
#include <fstream>
#include <optional>
#include <string>

#include "../common/types.hpp"
#include "packed_bytes.hpp"

namespace json
{
// ****************************
// *      parser declare      *
// ****************************

template <bool accept_jsonc = false, typename parsing_t = void, typename accel_traits = _packed_bytes::packed_bytes_trait_max>
class parser
{
public:
    using parsing_iter_t = typename parsing_t::const_iterator;

public:
    ~parser() noexcept = default;

    static std::optional<value> parse(const parsing_t& content);

private:
    parser(parsing_iter_t cbegin, parsing_iter_t cend) noexcept
        : _cur(cbegin)
        , _end(cend)
    {
        ;
    }

    std::optional<value> parse();
    value parse_value();

    value parse_null();
    value parse_boolean();
    value parse_number();
    // parse and return a value whose type is value_type::string
    value parse_string();
    value parse_array();
    value parse_object();

    // parse and return a std::string
    std::optional<std::string> parse_stdstring();

    bool skip_string_literal_with_accel();
    bool skip_whitespace() noexcept;
    bool skip_comment() noexcept;
    bool skip_digit();
    bool skip_unicode_escape(uint16_t& pair_high, std::string& result);

private:
    parsing_iter_t _cur;
    parsing_iter_t _end;
};

// ***************************
// *      utils declare      *
// ***************************

template <typename parsing_t>
std::optional<value> parse(const parsing_t& content);

std::optional<value> parse(const char* content);

template <typename parsing_t>
std::optional<value> parsec(const parsing_t& content);

std::optional<value> parsec(const char* content);

std::optional<value> parse(std::istream& istream, bool check_bom = false, bool with_commets = false);

template <typename path_t>
std::optional<value> open(const path_t& path, bool check_bom = false, bool with_commets = false);

namespace literals
{
value operator""_json(const char* str, size_t len);
value operator""_jvalue(const char* str, size_t len);

array operator""_jarray(const char* str, size_t len);

object operator""_jobject(const char* str, size_t len);
}

inline const value invalid_value();
} // namespace json
