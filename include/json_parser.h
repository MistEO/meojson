#pragma once

#include <string>
#include <regex>

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

        static value parse(const std::string &content);

    private:
        static value initial_parse(const std::string &content, std::string::const_iterator &cur);

        static value parse_string(const std::string &content, std::string::const_iterator &cur);
        static value parse_number(const std::string &content, std::string::const_iterator &cur);
        static value parse_null(const std::string &content, std::string::const_iterator &cur);
        static value parse_boolean(const std::string &content, std::string::const_iterator &cur);
        static object parse_object(const std::string &content, std::string::const_iterator &cur);
        static array parse_array(const std::string &content, std::string::const_iterator &cur);

        static value parse_by_regex(const std::string &content, std::string::const_iterator &cur, const std::regex &regex, ValueType type);
        static bool parse_whitespace(const std::string &content, std::string::const_iterator &cur);
        static std::string parse_string_and_return(const std::string &content, std::string::const_iterator &cur);
    };
} // namespace json