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

    auto &&[ret, result] = initial_parse(content, cur);
    if (!ret)
    {
        return std::make_pair(false, value());
    }

    // 解析完成后，后面不应再有除空格外的内容
    if (parse_whitespace(content, cur))
    {
        return std::make_pair(false, value());
    }

    return std::make_pair(true, std::forward<value>(result));
}

std::pair<bool, json::value> json::parser::initial_parse(const std::string &content, std::string::const_iterator &cur)
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
        if (parse_whitespace(content, cur))
        {
            return initial_parse(content, cur);
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
        return parse_array(content, cur);
    case '{':
        return parse_object(content, cur);
    default:
        return std::make_pair(false, value());
    }
}

std::pair<bool, json::value> json::parser::parse_null(const std::string &content, std::string::const_iterator &cur)
{
    static const std::string null_string = "null";

    if (static_cast<size_t>(std::distance(cur, content.cend())) >= null_string.size() &&
        std::string(cur, cur + null_string.size()) == null_string)
    {
        cur += null_string.size();
        return std::make_pair(true, value::null());
    }
    else
    {
        return std::make_pair(false, value());
    }
}

std::pair<bool, json::value> json::parser::parse_boolean(const std::string &content, std::string::const_iterator &cur)
{
    static const std::string true_string = "true";
    static const std::string false_string = "false";

    if (*cur == 't' &&
        static_cast<size_t>(std::distance(cur, content.cend())) >= true_string.size() &&
        std::string(cur, cur + true_string.size()) == true_string)
    {
        cur += true_string.size();
        return std::make_pair(true, value::boolean(true));
    }
    else if (*cur == 'f' &&
             static_cast<size_t>(std::distance(cur, content.cend())) >= false_string.size() &&
             std::string(cur, cur + false_string.size()) == false_string)
    {
        cur += false_string.size();
        return std::make_pair(true, value::boolean(false));
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

    static auto parse_digit = [&]() -> bool {
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
    };

    auto first = cur;
    if (*cur == '-')
    {
        ++cur;
    }
    if (!parse_digit())
    {
        return std::make_pair(false, value());
    }

    if (*cur == '.')
    {
        ++cur;
        if (!parse_digit())
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
            if (!parse_digit())
            {
                return std::make_pair(false, value());
            }
        }
    }

    std::string num_str(first, cur);
    value result;
    result.set_raw_basic_data(value_type::Number, std::move(num_str));
    return std::make_pair(true, std::move(result));

    // 正则太慢了，弃用之~~~
    // static const std::string reg_str_json_number("(-?\\d+(?:\\.\\d+)?(?:(?:e|E)(?:-|\\+)?\\d+)?)");
    // static const std::regex reg_json_number("^" + reg_str_json_number);

    // std::string cur_string(cur, content.cend());
    // std::smatch match_result;
    // if (std::regex_search(cur_string, match_result, reg_json_number) && match_result.size() == 2)
    // {
    //     std::string num = match_result[1];
    //     cur += num.size();

    //     value result;
    //     result.set_raw_basic_data(value_type::Number, std::move(num));
    //     return std::make_pair(true, std::move(result));
    // }
    // else
    // {
    //     return std::make_pair(false, value());
    // }
}

std::pair<bool, json::value>
json::parser::parse_string(const std::string &content, std::string::const_iterator &cur)
{
    auto &&[ret, str] = parse_string_str(content, cur);
    if (!ret)
    {
        return std::make_pair(false, value());
    }
    return std::make_pair(true, json::value::string(str));
}

std::pair<bool, json::array> json::parser::parse_array(const std::string &content, std::string::const_iterator &cur)
{
    if (cur != content.cend() && *cur == '[')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, array());
    }

    if (!parse_whitespace(content, cur))
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
        if (!parse_whitespace(content, cur))
        {
            return std::make_pair(false, array());
        }

        auto &&[ret, val] = initial_parse(content, cur);

        if (!parse_whitespace(content, cur))
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

    if (parse_whitespace(content, cur) && *cur == ']')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, array());
    }
    return std::make_pair(true, std::move(result));
}

std::pair<bool, json::object> json::parser::parse_object(const std::string &content, std::string::const_iterator &cur)
{
    if (cur != content.cend() && *cur == '{')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, object());
    }

    if (!parse_whitespace(content, cur))
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
        if (!parse_whitespace(content, cur))
        {
            return std::make_pair(false, object());
        }

        auto &&[key_ret, key] = parse_string_str(content, cur);
        if (!key_ret)
        {
            return std::make_pair(false, object());
        }

        if (parse_whitespace(content, cur) && *cur == ':')
        {
            ++cur;
        }
        else
        {
            return std::make_pair(false, object());
        }

        if (!parse_whitespace(content, cur))
        {
            return std::make_pair(false, object());
        }

        auto &&[val_ret, val] = initial_parse(content, cur);
        if (!val_ret)
        {
            return std::make_pair(false, object());
        }

        if (!parse_whitespace(content, cur))
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

    if (parse_whitespace(content, cur) && *cur == '}')
    {
        ++cur;
    }
    else
    {
        return std::make_pair(false, object());
    }
    return std::make_pair(true, std::move(result));
}

bool json::parser::parse_whitespace(const std::string &content, std::string::const_iterator &cur) noexcept
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

std::pair<bool, std::string> json::parser::parse_string_str(const std::string &content, std::string::const_iterator &cur)
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