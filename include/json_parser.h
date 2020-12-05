#pragma once

#include <string>
#include <optional>

namespace json
{
    class value;
    class object;
    class array;
    enum class value_type : char;

    class parser
    {
    public:
        parser() = delete;
        ~parser() noexcept = default;

        static std::optional<value> parse(const std::string &content);

    private:
        static value parse_value(const std::string &content, std::string::const_iterator &cur);

        static value parse_null(const std::string &content, std::string::const_iterator &cur);
        static value parse_boolean(const std::string &content, std::string::const_iterator &cur);
        static value parse_number(const std::string &content, std::string::const_iterator &cur);
        // parse and return a json::value whose type is value_type::String
        static value parse_string(const std::string &content, std::string::const_iterator &cur);
        static value parse_array(const std::string &content, std::string::const_iterator &cur);
        static value parse_object(const std::string &content, std::string::const_iterator &cur);

        // parse and return a std::string
        static std::optional<std::string> parse_stdstring(const std::string &content, std::string::const_iterator &cur);

        static bool skip_whitespace(const std::string &content, std::string::const_iterator &cur) noexcept;
        static bool skip_digit(const std::string &content, std::string::const_iterator &cur) noexcept;
    };
} // namespace json