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

// Reference
// Json     https://www.json.org/json-zh.html
// Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
const std::string json::parser::reg_str_json_whitespace("(?:\\s*)");

const std::string json::parser::reg_str_json_null("(null)");

const std::string json::parser::reg_str_json_boolean("(true|false)");

const std::string json::parser::reg_str_json_string("(\"[^\"]*\")");

const std::string json::parser::reg_str_json_number_fraction("(?:\\.\\d+)?");
const std::string json::parser::reg_str_json_number_exponent("(?:(?:e|E)(?:-|\\+)?\\d+)?");
const std::string json::parser::reg_str_json_number("(-?\\d+" + json::parser::reg_str_json_number_fraction + json::parser::reg_str_json_number_exponent + ")");

const std::string json::parser::reg_str_json_non_nested_value("(?:" + json::parser::reg_str_json_null + "|" + json::parser::reg_str_json_boolean + "|" + json::parser::reg_str_json_string + "|" + json::parser::reg_str_json_number + ")");

const std::regex json::parser::reg_json_null("^" + json::parser::reg_str_json_null);
const std::regex json::parser::reg_json_boolean("^" + json::parser::reg_str_json_boolean);
const std::regex json::parser::reg_json_string("^" + json::parser::reg_str_json_string);
const std::regex json::parser::reg_json_number("^" + json::parser::reg_str_json_number);
const std::regex json::parser::reg_json_non_nested_value("^" + json::parser::reg_str_json_non_nested_value);

json::value json::parser::parse(const std::string &content)
{
    auto parse_iter = content.cbegin();
    return parse_value(content, parse_iter);
}

json::value json::parser::parse_value(const std::string &content, std::string::const_iterator &cur)
{
    parse_whitespace(content, cur);
    if (cur == content.cend())
    {
        return json::value();
    }

    bool maybe_value = false;
    json::value::ValueType maybe_type = json::value::JsonInvalid;
    std::regex cur_regex;
    json::value parse_result;
    switch (*cur)
    {
    case '{':
        parse_result = parse_object(content, cur);
        maybe_type = json::value::JsonObject;
        break;
    case '[':
        parse_result = parse_array(content, cur);
        maybe_type = json::value::JsonArray;
        break;
    case 'n':
        maybe_value = true;
        maybe_type = json::value::JsonNull;
        cur_regex = reg_json_null;
        break;
    case '"':
        maybe_value = true;
        maybe_type = json::value::JsonString;
        cur_regex = reg_json_string;
        break;
    case 't':
    case 'f':
        maybe_value = true;
        maybe_type = json::value::JsonBoolean;
        cur_regex = reg_json_boolean;
        break;
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
        maybe_value = true;
        maybe_type = json::value::JsonNumber;
        cur_regex = reg_json_number;
        break;
    default:
        maybe_value = false;
        maybe_type = json::value::JsonInvalid;
        throw exception("Parsing error: " + std::string(cur, content.cend()));
        break;
    }

    if (maybe_value)
    {
        std::string cur_string(cur, content.cend());
        std::smatch match_result;
        if (std::regex_search(cur_string, match_result, cur_regex) && match_result.size() == 2)
        {
            std::string val = match_result[1];
            cur += val.size();
            parse_result.set_raw_data(maybe_type, val);
        }
        else
        {
            throw exception("Parsing error: " + cur_string);
        }
    }

    return parse_result;
}

json::object json::parser::parse_object(const std::string &content, std::string::const_iterator &cur)
{
    parse_whitespace(content, cur);
    if (cur == content.cend())
    {
        return json::object();
    }

    if (*cur == '{')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing error: " + std::string(cur, content.cend()));
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

        std::string cur_string(cur, content.cend());
        std::smatch match_result;
        std::string key;
        if (std::regex_search(cur_string, match_result, reg_json_string) && match_result.size() == 2)
        {
            key = match_result[1];
            cur += key.size();
            key = key.substr(1, key.size() - 2);
        }
        else
        {
            throw exception("Parsing error: " + std::string(cur, content.cend()));
        }

        parse_whitespace(content, cur);

        if (*cur == ':')
        {
            ++cur;
        }
        else
        {
            throw exception("Parsing error: " + std::string(cur, content.cend()));
        }

        parse_whitespace(content, cur);
        json::value val = parse_value(content, cur);
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
        throw exception("Parsing error: " + std::string(cur, content.cend()));
    }
    return parse_result_object;
}

json::array json::parser::parse_array(const std::string &content, std::string::const_iterator &cur)
{
    parse_whitespace(content, cur);
    if (cur == content.cend())
    {
        return json::array();
    }

    if (*cur == '[')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing error: " + std::string(cur, content.cend()));
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
        json::value val = parse_value(content, cur);
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
        throw exception("Parsing error: " + std::string(cur, content.cend()));
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