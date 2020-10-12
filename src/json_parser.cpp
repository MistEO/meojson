#include "json_parser.h"

#include <regex>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

/*** 
 * Reference
 * Json     https://www.json.org/json-zh.html
 * Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
***/
// const std::string json::parser::reg_str_json_whitespace("(?:\\s*)");
const std::string json::parser::reg_str_json_null("(null)");
const std::string json::parser::reg_str_json_boolean("(true|false)");
// const std::string json::parser::reg_str_json_string("(\"[^\"]*\")"); // 弃用，无法满足"\""的情况
const std::string json::parser::reg_str_json_number_fraction("(?:\\.\\d+)?");
const std::string json::parser::reg_str_json_number_exponent("(?:(?:e|E)(?:-|\\+)?\\d+)?");
const std::string json::parser::reg_str_json_number("(-?\\d+" + json::parser::reg_str_json_number_fraction + json::parser::reg_str_json_number_exponent + ")");
// const std::string json::parser::reg_str_json_non_nested_value("(?:" + json::parser::reg_str_json_null + "|" + json::parser::reg_str_json_boolean + "|" + json::parser::reg_str_json_string + "|" + json::parser::reg_str_json_number + ")");

const std::regex json::parser::reg_json_null("^" + json::parser::reg_str_json_null);
const std::regex json::parser::reg_json_boolean("^" + json::parser::reg_str_json_boolean);
// const std::regex json::parser::reg_json_string("^" + json::parser::reg_str_json_string);
const std::regex json::parser::reg_json_number("^" + json::parser::reg_str_json_number);
// const std::regex json::parser::reg_json_non_nested_value("^" + json::parser::reg_str_json_non_nested_value);

json::value json::parser::parse(const std::string &content)
{
    auto cur = content.cbegin();
    parse_whitespace(content, cur);
    if (cur == content.cend())
    {
        return json::value();
    }
    return initial_parse(content, cur);
}

json::value json::parser::initial_parse(const std::string &content, std::string::const_iterator &cur)
{
    switch (*cur)
    {
    case '{':
        return parse_object(content, cur);
    case '[':
        return parse_array(content, cur);
    case '"':
        return parse_string(content, cur);
    case 'n':
        return parse_by_regex(content, cur, reg_json_null, JsonNull);
    case 't':
    case 'f':
        return parse_by_regex(content, cur, reg_json_boolean, JsonBoolean);
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
        return parse_by_regex(content, cur, reg_json_number, JsonNumber);
    default:
        throw json::exception("Parsing error: " + std::string(cur, content.cend()));
    }
}

json::value json::parser::parse_by_regex(const std::string &content, std::string::const_iterator &cur, const std::regex &regex, json::ValueType type)
{
    std::string cur_string(cur, content.cend());
    std::smatch match_result;
    json::value parse_result;
    if (std::regex_search(cur_string, match_result, regex) && match_result.size() == 2)
    {
        std::string val = match_result[1];
        cur += val.size();
        parse_result.set_raw_basic_data(type, val);
    }
    else
    {
        throw json::exception("Parsing regex " + std::to_string(type) + " error: " + cur_string);
    }
    return parse_result;
}

json::value json::parser::parse_string(const std::string &content, std::string::const_iterator &cur)
{
    return json::value::string(parse_string_and_return(content, cur));
}

std::string json::parser::parse_string_and_return(const std::string &content, std::string::const_iterator &cur)
{
    auto first = cur;
    if (*cur == '"')
    {
        ++cur;
    }
    else
    {
        throw json::exception("Parsing string error: " + std::string(cur, content.cend()));
    }

    while (true)
    {
        if (cur == content.cend())
        {
            throw json::exception("Parsing string error: " + std::string(cur, content.cend()));
        }
        if (*cur == '"' && *(cur - 1) != '\\')
        {
            ++cur;
            break;
        }
        ++cur;
    }
    return std::string(first, cur);
}

json::object json::parser::parse_object(const std::string &content, std::string::const_iterator &cur)
{
    if (*cur == '{')
    {
        ++cur;
    }
    else
    {
        throw json::exception("Parsing object error: " + std::string(cur, content.cend()));
    }

    parse_whitespace(content, cur);

    if (*cur == ']')
    {
        ++cur;
        return json::object();
    }

    json::object parse_result_object;
    while (true)
    {
        parse_whitespace(content, cur);

        std::string key_temp = parse_string_and_return(content, cur);
        std::string key = key_temp.substr(1, key_temp.size() - 2);

        parse_whitespace(content, cur);

        if (*cur == ':')
        {
            ++cur;
        }
        else
        {
            throw json::exception("Parsing object error: " + std::string(cur, content.cend()));
        }

        parse_whitespace(content, cur);
        json::value val = initial_parse(content, cur);
        parse_whitespace(content, cur);

        parse_result_object.insert(key, val);

        if (*cur == ',')
        {
            ++cur;
        }
        else
        {
            break;
        }
    }

    parse_whitespace(content, cur);
    if (*cur == '}')
    {
        ++cur;
    }
    else
    {
        throw json::exception("Parsing object error: " + std::string(cur, content.cend()));
    }
    return parse_result_object;
}

json::array json::parser::parse_array(const std::string &content, std::string::const_iterator &cur)
{
    if (*cur == '[')
    {
        ++cur;
    }
    else
    {
        throw json::exception("Parsing array error: " + std::string(cur, content.cend()));
    }

    parse_whitespace(content, cur);
    if (*cur == ']')
    {
        ++cur;
        return json::array();
    }

    json::array parse_result_array;
    while (true)
    {
        parse_whitespace(content, cur);
        json::value val = initial_parse(content, cur);
        parse_whitespace(content, cur);

        parse_result_array.push_back(val);

        if (*cur == ',')
        {
            ++cur;
        }
        else
        {
            break;
        }
    }

    parse_whitespace(content, cur);
    if (*cur == ']')
    {
        ++cur;
    }
    else
    {
        throw json::exception("Parsing array error: " + std::string(cur, content.cend()));
    }
    return parse_result_array;
}

void json::parser::parse_whitespace(const std::string &content, std::string::const_iterator &cur)
{
    while (cur != content.cend() && (*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n'))
    {
        ++cur;
    }
}