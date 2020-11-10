#pragma once

#include <string>
#include <utility>

namespace json
{
    class value;
    class object;
    class array;
    enum class ValueType;

    class parser
    {
    public:
        parser() = delete;
        ~parser() = default;

        static std::pair<bool, value> parse(const std::string &content);

    private:
        static std::pair<bool, value> initial_parse(const std::string &content, std::string::const_iterator &cur);

        static std::pair<bool, value> parse_null(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, value> parse_boolean(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, value> parse_number(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, value> parse_string(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, array> parse_array(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, object> parse_object(const std::string &content, std::string::const_iterator &cur);

        static bool parse_whitespace(const std::string &content, std::string::const_iterator &cur);
        static std::pair<bool, std::string> parse_string_str(const std::string &content, std::string::const_iterator &cur);
    };
} // namespace json