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

json::value json::parser::parse(const std::string &content)
{
    auto cur = content.cbegin();
    bool ws_ret = parse_whitespace(content, cur);
    if (!ws_ret)
    {
        return value();
    }
    auto value = initial_parse(content, cur);
    ws_ret = parse_whitespace(content, cur);

    if (!ws_ret)
    {
        return value;
    }
    else
    {
        throw exception("Parsing error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
}

/*** 
 * Reference
 * Json     https://www.json.org/json-zh.html
***/
json::value json::parser::initial_parse(const std::string &content, std::string::const_iterator &cur)
{
    if (cur == content.cend())
    {
        throw exception("Parsing error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
    switch (*cur)
    {
    case '{':
        return parse_object(content, cur);
    case '[':
        return parse_array(content, cur);
    case '"':
        return parse_string(content, cur);
    case 'n':
        return parse_null(content, cur);
    case 't':
    case 'f':
        return parse_boolean(content, cur);
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
        return parse_number(content, cur);
    default:
        throw exception("Parsing error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
}

json::value json::parser::parse_by_regex(const std::string &content, std::string::const_iterator &cur, const std::regex &regex, json::ValueType type)
{
    if (cur == content.cend())
    {
        throw exception("Parsing regex " + std::to_string(static_cast<int>(type)) + " error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
    std::string cur_string(cur, content.cend());
    std::smatch match_result;
    value parse_result;
    if (std::regex_search(cur_string, match_result, regex) && match_result.size() == 2)
    {
        std::string val = match_result[1];
        cur += val.size();
        parse_result.set_raw_basic_data(type, std::move(val));
    }
    else
    {
        throw exception("Parsing regex " + std::to_string(static_cast<int>(type)) + " error: " + cur_string);
    }
    return parse_result;
}

json::value json::parser::parse_string(const std::string &content, std::string::const_iterator &cur)
{
    return value::string(parse_string_and_return(content, cur));
}

std::string json::parser::parse_string_and_return(const std::string &content, std::string::const_iterator &cur)
{
    if (cur != content.cend() && *cur == '"')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing string error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }

    const auto first = cur;
    auto last = cur;
    while (true)
    {
        if (cur == content.cend())
        {
            throw exception("Parsing string error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }
        else if (*cur == '"' && *(cur - 1) != '\\')
        {
            last = cur;
            ++cur;
            break;
        }
        ++cur;
    }
    return std::string(first, last);
}

json::value json::parser::parse_number(const std::string &content, std::string::const_iterator &cur)
{
    static const std::string reg_str_json_number("(-?\\d+(?:\\.\\d+)?(?:(?:e|E)(?:-|\\+)?\\d+)?)");
    static const std::regex reg_json_number("^" + reg_str_json_number);

    return parse_by_regex(content, cur, reg_json_number, ValueType::JsonNumber);
}

json::value json::parser::parse_boolean(const std::string &content, std::string::const_iterator &cur)
{
    static const std::string true_string = "true";
    static const std::string false_string = "false";
    if (*cur == 't' &&
        static_cast<size_t>(std::distance(cur, content.cend())) >= true_string.size() &&
        std::string(cur, cur + true_string.size()) == true_string)
    {
        cur += true_string.size();
        return value::boolean(true);
    }
    else if (*cur == 'f' &&
             static_cast<size_t>(std::distance(cur, content.cend())) >= false_string.size() &&
             std::string(cur, cur + false_string.size()) == false_string)
    {
        cur += false_string.size();
        return value::boolean(false);
    }
    else
    {
        throw exception("Parsing boolean error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
}

json::value json::parser::parse_null(const std::string &content, std::string::const_iterator &cur)
{
    static const std::string null_string = "null";
    if (static_cast<size_t>(std::distance(cur, content.cend())) >= null_string.size() &&
        std::string(cur, cur + null_string.size()) == null_string)
    {
        cur += null_string.size();
        return value::null();
    }
    else
    {
        throw exception("Parsing null error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
}

json::object json::parser::parse_object(const std::string &content, std::string::const_iterator &cur)
{
    if (cur != content.cend() && *cur == '{')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }

    bool ws_ret = parse_whitespace(content, cur);
    if (!ws_ret)
    {
        throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
    else if (*cur == '}')
    {
        ++cur;
        return object();
    }

    object parse_result_object;
    while (true)
    {
        ws_ret = parse_whitespace(content, cur);
        if (!ws_ret)
        {
            throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }

        std::string key = parse_string_and_return(content, cur);

        ws_ret = parse_whitespace(content, cur);

        if (ws_ret && *cur == ':')
        {
            ++cur;
        }
        else
        {
            throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }

        ws_ret = parse_whitespace(content, cur);
        if (!ws_ret)
        {
            throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }

        value val = initial_parse(content, cur);

        ws_ret = parse_whitespace(content, cur);
        if (!ws_ret)
        {
            throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }

        parse_result_object.insert(std::move(key), std::move(val));
        if (*cur == ',')
        {
            ++cur;
        }
        else
        {
            break;
        }
    }

    ws_ret = parse_whitespace(content, cur);
    if (ws_ret && *cur == '}')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing object error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
    return parse_result_object;
}

json::array json::parser::parse_array(const std::string &content, std::string::const_iterator &cur)
{
    if (cur != content.cend() && *cur == '[')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing array error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }

    bool ws_ret = parse_whitespace(content, cur);
    if (!ws_ret)
    {
        throw exception("Parsing array error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
    else if (*cur == ']')
    {
        ++cur;
        return array();
    }

    array parse_result_array;
    while (true)
    {
        ws_ret = parse_whitespace(content, cur);
        if (!ws_ret)
        {
            throw exception("Parsing array error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }

        value val = initial_parse(content, cur);

        ws_ret = parse_whitespace(content, cur);
        if (!ws_ret)
        {
            throw exception("Parsing array error: " + std::to_string(std::distance(content.cbegin(), cur)));
        }

        parse_result_array.push_back(std::move(val));

        if (*cur == ',')
        {
            ++cur;
        }
        else
        {
            break;
        }
    }

    ws_ret = parse_whitespace(content, cur);
    if (ws_ret && *cur == ']')
    {
        ++cur;
    }
    else
    {
        throw exception("Parsing array error: " + std::to_string(std::distance(content.cbegin(), cur)));
    }
    return parse_result_array;
}

bool json::parser::parse_whitespace(const std::string &content, std::string::const_iterator &cur)
{
    while (cur != content.cend() && (*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n'))
    {
        ++cur;
    }

    if (cur != content.cend())
    {
        return true;
    }
    else
    {
        return false;
    }
}