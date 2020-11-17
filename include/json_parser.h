#pragma once

#include <string>
#include <utility>
#include <limits>

namespace json
{
    class value;
    class object;
    class array;
    enum class value_type;

    class parser
    {
    public:
        parser() = delete;
        ~parser() noexcept = default;

        static std::pair<bool, value> parse(const std::string &content, size_t lazy_depth = 1);

    private:
        static std::pair<bool, value> parse_value(const std::string &content, std::string::const_iterator &cur, size_t lazy_depth);

        static std::pair<bool, value> parse_null(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, value> parse_boolean(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, value> parse_number(const std::string &content, std::string::const_iterator &cur);
        // parse and return a json::value whose type is value_type::String
        static std::pair<bool, value> parse_string(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, value> parse_array(const std::string &content, std::string::const_iterator &cur, size_t lazy_depth);
        static std::pair<bool, value> parse_object(const std::string &content, std::string::const_iterator &cur, size_t lazy_depth);

        // parse and return a std::string
        static std::pair<bool, std::string> parse_str(const std::string &content, std::string::const_iterator &cur);

        static bool skip_whitespace(const std::string &content, std::string::const_iterator &cur) noexcept;
        static bool skip_digit(const std::string &content, std::string::const_iterator &cur) noexcept;
    };
} // namespace json