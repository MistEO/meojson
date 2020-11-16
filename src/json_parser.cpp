#include "json_parser.h"

#include <regex>

#include "json_value.h"
#include "json_object.h"
#include "json_array.h"

/*** 
 * Reference
 * Json     https://www.json.org/json-zh.html
***/

std::pair<bool, json::value> json::parser::parse(const std::string &content)
{
    auto cur = content.cbegin();

    if (!skip_whitespace(content, cur))
    {
        return std::make_pair(false, value::invalid_value());
    }

    auto &&[ret, result] = parse_value(content, cur, SIZE_MAX);
    if (!ret)
    {
        return std::make_pair(false, value::invalid_value());
    }

    // 解析完成后，后面不应再有除空格外的内容
    if (skip_whitespace(content, cur))
    {
        return std::make_pair(false, value::invalid_value());
    }

    return std::make_pair(true, std::forward<value>(result));
}

std::pair<bool, json::value> json::parser::lazy_parse(const std::string &content, size_t max_depth)
{
    auto cur = content.cbegin();

    if (!skip_whitespace(content, cur))
    {
        return std::make_pair(false, value::invalid_value());
    }

    auto &&[ret, result] = parse_value(content, cur, max_depth);
    if (!ret)
    {
        return std::make_pair(false, value::invalid_value());
    }

    // 解析完成后，后面不应再有除空格外的内容
    if (skip_whitespace(content, cur))
    {
        return std::make_pair(false, value::invalid_value());
    }

    return std::make_pair(true, std::forward<value>(result));
}

std::pair<bool, json::value> json::parser::parse_value(const std::string &content, std::string::const_iterator &cur, size_t lazy_depth)
{
    if (cur == content.cend())
    {
        return std::make_pair(false, value());
    }

    switch (*cur)
    {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        if (skip_whitespace(content, cur))
        {
            return parse_value(content, cur, lazy_depth);
        }
        else
        {
            return std::make_pair(false, value());
        }
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
    case '"':
        return parse_string(content, cur);
    case '[':
        if (lazy_depth > 0)
        {
            return parse_array(content, cur, lazy_depth - 1);
        }
        else
        {
            const auto frist = cur;
            bool ret = skip_array(content, cur);
            return std::make_pair(ret, value(value_type::Array, std::string(frist, cur)));
        }
    case '{':
        if (lazy_depth > 0)
        {
            return parse_object(content, cur, lazy_depth - 1);
        }
        else
        {
            const auto frist = cur;
            bool ret = skip_object(content, cur);
            return std::make_pair(ret, value(value_type::Object, std::string(frist, cur)));
        }
    default:
        return std::make_pair(false, value());
    }
}

std::pair<bool, json::value> json::parser::parse_null(const std::string &content, std::string::const_iterator &cur)
{
    if (cur == content.cend())
    {
        return std::make_pair(false, value());
    }

    static const std::string null_string = "null";

    for (auto &&iter : null_string)
    {
        if (cur != content.end() && *cur == iter)
        {
            ++cur;
        }
        else
        {
            return std::make_pair(false, value());
        }
    }

    return std::make_pair(true, value());
}

std::pair<bool, json::value> json::parser::parse_boolean(const std::string &content, std::string::const_iterator &cur)
{
    if (cur == content.cend())
    {
        return std::make_pair(false, value());
    }

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
                return std::make_pair(false, value());
            }
        }
        return std::make_pair(true, value(true));
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
                return std::make_pair(false, value());
            }
        }
        return std::make_pair(true, value(false));
    }
    else
    {
        return std::make_pair(false, value());
    }
}

std::pair<bool, json::value> json::parser::parse_number(const std::string &content, std::string::const_iterator &cur)
{
    if (cur == content.cend())
    {
        return std::make_pair(false, value());
    }

    const auto first = cur;
    if (*cur == '-')
    {
        ++cur;
    }
    if (!skip_digit(content, cur))
    {
        return std::make_pair(false, value());
    }

    if (*cur == '.')
    {
        ++cur;
        if (!skip_digit(content, cur))
        {
            return std::make_pair(false, value());
        }
    }

    if (*cur == 'e' || *cur == 'E')
    {
        ++cur;
        if (*cur == '+' || *cur == '-')
        {
            ++cur;
            if (!skip_digit(content, cur))
            {
                return std::make_pair(false, value());
            }
        }
    }

    return std::make_pair(true, value(value_type::Number, std::string(first, cur)));
}

std::pair<bool, json::value>
json::parser::parse_string(const std::string &content, std::string::const_iterator &cur)
{
    auto &&[ret, str] = parse_str(content, cur);
    if (!ret)
    {
        return std::make_pair(false, value());
    }
    return std::make_pair(true, value(std::forward<std::string>(str)));
}

std::pair<bool, json::array> json::parser::parse_array(const std::string &content, std::string::const_iterator &cur, size_t lazy_depth)
{
    if (cur != content.cend() && *cur == '[')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, array());
    }

    if (!skip_whitespace(content, cur))
    {
        return std::make_pair(false, array());
    }
    else if (*cur == ']')
    {
        ++cur;
        // empty array
        return std::make_pair(true, array());
    }

    array result;
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return std::make_pair(false, array());
        }

        auto &&[ret, val] = parse_value(content, cur, lazy_depth);

        if (!skip_whitespace(content, cur))
        {
            return std::make_pair(false, array());
        }

        result.push_back(std::forward<value>(val));

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
        return std::make_pair(false, array());
    }
    return std::make_pair(true, std::move(result));
}

std::pair<bool, json::object> json::parser::parse_object(const std::string &content, std::string::const_iterator &cur, size_t lazy_depth)
{
    if (cur != content.cend() && *cur == '{')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, object());
    }

    if (!skip_whitespace(content, cur))
    {
        return std::make_pair(false, object());
    }
    else if (*cur == '}')
    {
        ++cur;
        // empty object
        return std::make_pair(true, object());
    }

    object result;
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return std::make_pair(false, object());
        }

        auto &&[key_ret, key] = parse_str(content, cur);
        if (!key_ret)
        {
            return std::make_pair(false, object());
        }

        if (skip_whitespace(content, cur) && *cur == ':')
        {
            ++cur;
        }
        else
        {
            return std::make_pair(false, object());
        }

        if (!skip_whitespace(content, cur))
        {
            return std::make_pair(false, object());
        }

        auto &&[val_ret, val] = parse_value(content, cur, lazy_depth);
        if (!val_ret)
        {
            return std::make_pair(false, object());
        }

        if (!skip_whitespace(content, cur))
        {
            return std::make_pair(false, object());
        }

        result.insert(std::forward<std::string>(key), std::forward<value>(val));
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
        return std::make_pair(false, object());
    }
    return std::make_pair(true, std::move(result));
}

std::pair<bool, std::string> json::parser::parse_str(const std::string &content, std::string::const_iterator &cur)
{
    if (cur != content.cend() && *cur == '"')
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
        if (cur == content.cend())
        {
            return std::make_pair(false, std::string());
        }
        else if (*cur == '"' && *(cur - 1) != '\\')
        {
            last = cur;
            ++cur;
            break;
        }
        ++cur;
    }
    return std::make_pair(true, std::string(first, last));
}

bool json::parser::skip_whitespace(const std::string &content, std::string::const_iterator &cur) noexcept
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

bool json::parser::skip_digit(const std::string &content, std::string::const_iterator &cur) noexcept
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

bool json::parser::skip_value(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur == content.cend())
    {
        return false;
    }
    switch (*cur)
    {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        if (skip_whitespace(content, cur))
        {
            return skip_value(content, cur);
        }
        else
        {
            return false;
        }
    case 'n':
        return skip_null(content, cur);
    case 't':
    case 'f':
        return skip_boolean(content, cur);
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
        return skip_number(content, cur);
    case '"':
        return skip_string(content, cur);
    case '[':
        return skip_array(content, cur);
    case '{':
        return skip_object(content, cur);
    default:
        return false;
    }
}

bool json::parser::skip_null(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur == content.cend())
    {
        return false;
    }

    static const std::string null_string = "null";

    for (auto &&iter : null_string)
    {
        if (cur != content.end() && *cur == iter)
        {
            ++cur;
        }
        else
        {
            return false;
        }
    }

    return true;
}

bool json::parser::skip_boolean(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur == content.cend())
    {
        return false;
    }

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
                return false;
            }
        }
        return true;
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
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

bool json::parser::skip_number(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur == content.cend())
    {
        return false;
    }

    if (*cur == '-')
    {
        ++cur;
    }
    if (!skip_digit(content, cur))
    {
        return false;
    }

    if (*cur == '.')
    {
        ++cur;
        if (!skip_digit(content, cur))
        {
            return false;
        }
    }

    if (*cur == 'e' || *cur == 'E')
    {
        ++cur;
        if (*cur == '+' || *cur == '-')
        {
            ++cur;
            if (!skip_digit(content, cur))
            {
                return false;
            }
        }
    }

    return true;
}

bool json::parser::skip_string(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur != content.cend() && *cur == '"')
    {
        ++cur;
    }
    else
    {
        return false;
    }

    while (true)
    {
        if (cur == content.cend())
        {
            return false;
        }
        else if (*cur == '"' && *(cur - 1) != '\\')
        {
            ++cur;
            break;
        }
        ++cur;
    }
    return true;
}

bool json::parser::skip_array(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur != content.cend() && *cur == '[')
    {
        ++cur;
    }
    else
    {
        return false;
    }

    if (!skip_whitespace(content, cur))
    {
        return false;
    }
    else if (*cur == ']')
    {
        ++cur;
        // empty array
        return true;
    }
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return false;
        }

        if (!skip_value(content, cur))
        {
            return false;
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
        return false;
    }
    return true;
}

bool json::parser::skip_object(const std::string &content, std::string::const_iterator &cur) noexcept
{
    if (cur != content.cend() && *cur == '{')
    {
        ++cur;
    }
    else
    {
        return false;
    }

    if (!skip_whitespace(content, cur))
    {
        return false;
    }
    else if (*cur == '}')
    {
        ++cur;
        // empty object
        return true;
    }

    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return false;
        }

        if (!skip_string(content, cur))
        {
            return false;
        }

        if (skip_whitespace(content, cur) && *cur == ':')
        {
            ++cur;
        }
        else
        {
            return false;
        }

        if (!skip_whitespace(content, cur))
        {
            return false;
        }

        if (!skip_value(content, cur))
        {
            return false;
        }

        if (!skip_whitespace(content, cur))
        {
            return false;
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
        return false;
    }
    return true;
}