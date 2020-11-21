#include "json_parser.h"

#include <regex>

#include "json_value.h"
#include "json_object.h"
#include "json_array.h"

/*** 
 * Reference
 * Json     https://www.json.org/json-zh.html
***/

std::pair<bool, json::value> json::parser::parse(
    const std::string &content, size_t lazy_depth)
{
    auto cur = content.cbegin();

    if (!skip_whitespace(content, cur))
    {
        return std::make_pair(false, value::invalid_value());
    }

    value result_value;
    switch (*cur)
    {
    case '[':
        result_value = parse_array(content, cur, lazy_depth);
        break;
    case '{':
        result_value = parse_object(content, cur, lazy_depth);
        break;
    default: // A JSON payload should be an object or array
        return std::make_pair(false, value::invalid_value());
    }

    if (!result_value.valid())
    {
        return std::make_pair(false, value::invalid_value());
    }

    // 解析完成后，后面不应再有除空格外的内容
    if (skip_whitespace(content, cur))
    {
        return std::make_pair(false, value::invalid_value());
    }

    return std::make_pair(true, std::move(result_value));
}

json::value json::parser::parse_value(
    const std::string &content, std::string::const_iterator &cur, size_t lazy_depth)
{
    switch (*cur)
    {
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
        return parse_number(content, cur, lazy_depth);
    case '"':
        return parse_string(content, cur, lazy_depth);
    case '[':
        return parse_array(content, cur, lazy_depth - 1);
    case '{':
        return parse_object(content, cur, lazy_depth - 1);
    default:
        return value::invalid_value();
    }
}

json::value json::parser::parse_null(
    const std::string &content, std::string::const_iterator &cur)
{
    static const std::string null_string = "null";

    for (auto &&iter : null_string)
    {
        if (cur != content.end() && *cur == iter)
        {
            ++cur;
        }
        else
        {
            return value::invalid_value();
        }
    }

    return value();
}

json::value json::parser::parse_boolean(
    const std::string &content, std::string::const_iterator &cur)
{
    static const std::string true_string = "true";
    static const std::string false_string = "false";

    if (*cur == 't')
    {
        for (auto &&iter : true_string)
        {
            if (cur != content.end() && *cur == iter)
            {
                ++cur;
            }
            else
            {
                return value::invalid_value();
            }
        }
        return value(true);
    }
    else if (*cur == 'f')
    {
        for (auto &&iter : false_string)
        {
            if (cur != content.end() && *cur == iter)
            {
                ++cur;
            }
            else
            {
                return value::invalid_value();
            }
        }
        return value(false);
    }
    else
    {
        return value::invalid_value();
    }
}

json::value json::parser::parse_number(
    const std::string &content, std::string::const_iterator &cur, bool need_return)
{
    const auto first = cur;
    if (*cur == '-')
    {
        ++cur;
    }
    if (cur == content.cend() ||
        // Numbers cannot have leading zeroes
        (*cur == '0' && cur + 1 != content.cend() && *(cur + 1) >= '0' && *(cur + 1) <= '9') ||
        !skip_digit(content, cur))
    {
        return value::invalid_value();
    }

    if (*cur == '.')
    {
        ++cur;
        if (!skip_digit(content, cur))
        {
            return value::invalid_value();
        }
    }

    if (*cur == 'e' || *cur == 'E')
    {
        ++cur;
        if (*cur == '+' || *cur == '-')
        {
            ++cur;
        }
        if (!skip_digit(content, cur))
        {
            return value::invalid_value();
        }
    }

    if (!need_return)
    {
        return value();
    }
    else
    {
        return value(value_type::Number, std::string(first, cur));
    }
}

json::value json::parser::parse_string(
    const std::string &content, std::string::const_iterator &cur, bool need_return)
{
    auto &&[ret, str] = parse_str(content, cur, need_return);
    if (!ret)
    {
        return value::invalid_value();
    }

    return value(std::move(str));
}

json::value json::parser::parse_array(
    const std::string &content, std::string::const_iterator &cur, size_t lazy_depth)
{
    const auto first = cur;

    if (*cur == '[')
    {
        ++cur;
    }
    else
    {
        return value::invalid_value();
    }

    if (!skip_whitespace(content, cur))
    {
        return value::invalid_value();
    }
    else if (*cur == ']')
    {
        ++cur;
        // empty array
        return value(array());
    }

    array::raw_array result;
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        auto val = parse_value(content, cur, lazy_depth);

        if (!val.valid() || !skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        if (lazy_depth > 0)
        {
            result.emplace_back(std::move(val));
        }

        if (*cur == ',')
        {
            ++cur;
        }
        else
        {
            break;
        }
    }

    if (skip_whitespace(content, cur) && *cur == ']')
    {
        ++cur;
    }
    else
    {
        return value::invalid_value();
    }
    if (lazy_depth > 0)
    {
        return value(std::move(result));
    }
    else
    {
        return value(value_type::Array, std::string(first, cur));
    }
}

json::value json::parser::parse_object(
    const std::string &content, std::string::const_iterator &cur, size_t lazy_depth)
{
    const auto first = cur;

    if (*cur == '{')
    {
        ++cur;
    }
    else
    {
        return value::invalid_value();
    }

    if (!skip_whitespace(content, cur))
    {
        return value::invalid_value();
    }
    else if (*cur == '}')
    {
        ++cur;
        // empty object
        return value(object());
    }

    object::raw_object result;
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        auto &&[key_ret, key] = parse_str(content, cur, lazy_depth);

        if (key_ret && skip_whitespace(content, cur) && *cur == ':')
        {
            ++cur;
        }
        else
        {
            return value::invalid_value();
        }

        if (!skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        auto val = parse_value(content, cur, lazy_depth);

        if (!val.valid() || !skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }
        if (lazy_depth > 0)
        {
            result.emplace(std::move(key), std::move(val));
        }
        if (*cur == ',')
        {
            ++cur;
        }
        else
        {
            break;
        }
    }

    if (skip_whitespace(content, cur) && *cur == '}')
    {
        ++cur;
    }
    else
    {
        return value::invalid_value();
    }
    if (lazy_depth > 0)
    {
        return value(std::move(result));
    }
    else
    {
        return value(value_type::Object, std::string(first, cur));
    }
}

std::pair<bool, std::string> json::parser::parse_str(
    const std::string &content, std::string::const_iterator &cur, bool need_return)
{
    if (*cur == '"')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, std::string());
    }

    const auto first = cur;
    auto last = cur;
    while (true)
    {
        if (cur == content.cend() || *cur == '\t' || *cur == '\r' || *cur == '\n')
        {
            return std::make_pair(false, std::string());
        }
        else if (*cur == '\\') // 如果是转义，检查后面那个字符是不是合法转义
        {
            ++cur;
            if (cur == content.cend())
            {
                return std::make_pair(false, std::string());
            }
            switch (*cur)
            {
            case '"':
            case '\\':
            case '/':
            case 'b':
            case 'f':
            case 'n':
            case 'r':
            case 't':
            case 'u':
                // Do nothing, after break, `cur` self-increasing
                break;
            default:
                // Illegal backslash escape
                return std::make_pair(false, std::string());
            }
        }
        else if (*cur == '"')
        {
            last = cur;
            ++cur;
            break;
        }
        ++cur;
    }
    if (!need_return)
    {
        return std::make_pair(true, std::string());
    }
    else
    {
        return std::make_pair(true, std::string(first, last));
    }
}

bool json::parser::skip_whitespace(
    const std::string &content, std::string::const_iterator &cur) noexcept
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

bool json::parser::skip_digit(
    const std::string &content, std::string::const_iterator &cur) noexcept
{
    // 至少要有一个数字
    if (cur != content.cend() && *cur >= '0' && *cur <= '9')
    {
        ++cur;
    }
    else
    {
        return false;
    }

    while (cur != content.cend() && *cur >= '0' && *cur <= '9')
    {
        ++cur;
    }
    return true;
}
