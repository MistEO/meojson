#include "json_parser.h"

#include "json_value.h"
#include "json_object.h"
#include "json_array.h"

/*** 
 * Reference
 * Json     https://www.json.org/json-zh.html
***/

std::optional<json::value> json::parser::parse(
    const std::string &content)
{
    auto cur = content.cbegin();

    if (!skip_whitespace(content, cur))
    {
        return std::nullopt;
    }

    value result_value;
    switch (*cur)
    {
    case '[':
        result_value = parse_array(content, cur);
        break;
    case '{':
        result_value = parse_object(content, cur);
        break;
    default: // A JSON payload should be an object or array
        return std::nullopt;
    }

    if (!result_value.valid())
    {
        return std::nullopt;
    }

    // 解析完成后，后面不应再有除空格外的内容
    if (skip_whitespace(content, cur))
    {
        return std::nullopt;
    }

    return result_value;
}

json::value json::parser::parse_value(
    const std::string &content, std::string::const_iterator &cur)
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
        return parse_number(content, cur);
    case '"':
        return parse_string(content, cur);
    case '[':
        return parse_array(content, cur);
    case '{':
        return parse_object(content, cur);
    default:
        return value::invalid_value();
    }
}

json::value json::parser::parse_null(
    const std::string &content, std::string::const_iterator &cur)
{
    static const std::string null_string = "null";

    for (auto &&ch : null_string)
    {
        if (*cur == ch)
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

    switch (*cur)
    {
    case 't':
        for (auto &&ch : true_string)
        {
            if (*cur == ch)
            {
                ++cur;
            }
            else
            {
                return value::invalid_value();
            }
        }
        return true;
    case 'f':
        for (auto &&ch : false_string)
        {
            if (*cur == ch)
            {
                ++cur;
            }
            else
            {
                return value::invalid_value();
            }
        }
        return false;
    default:
        return value::invalid_value();
    }
}

json::value json::parser::parse_number(
    const std::string &content, std::string::const_iterator &cur)
{
    const auto first = cur;
    if (*cur == '-')
    {
        ++cur;
    }

    // Numbers cannot have leading zeroes
    if (*cur == '0' && isdigit(*(cur + 1)))
    {
        return value::invalid_value();
    }

    if (!skip_digit(content, cur))
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

    return value(value_type::Number, first, cur);
}

json::value json::parser::parse_string(
    const std::string &content, std::string::const_iterator &cur)
{
    auto string_opt = parse_stdstring(content, cur);
    if (!string_opt)
    {
        return value::invalid_value();
    }

    return std::move(string_opt).value();
}

json::value json::parser::parse_array(
    const std::string &content, std::string::const_iterator &cur)
{
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
        return array();
    }

    array result;
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        auto val = parse_value(content, cur);

        if (!val.valid() || !skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        result.emplace_back(std::move(val));

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

    return result;
}

json::value json::parser::parse_object(
    const std::string &content, std::string::const_iterator &cur)
{
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
        return object();
    }

    object result;
    while (true)
    {
        if (!skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        auto key_opt = parse_stdstring(content, cur);

        if (key_opt && skip_whitespace(content, cur) && *cur == ':')
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

        auto val = parse_value(content, cur);

        if (!val.valid() || !skip_whitespace(content, cur))
        {
            return value::invalid_value();
        }

        result.emplace(std::move(key_opt).value(), std::move(val));

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

    return result;
}

std::optional<std::string> json::parser::parse_stdstring(
    const std::string &content, std::string::const_iterator &cur)
{
    if (*cur == '"')
    {
        ++cur;
    }
    else
    {
        return std::nullopt;
    }

    const auto first = cur;
    auto last = cur;
    bool is_string_end = false;
    while (!is_string_end)
    {
        switch (*cur)
        {
        case '\t':
        case '\r':
        case '\n':
        case '\0': // std::string::end();
            return std::nullopt;
        case '\\':
        {
            ++cur;
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
                ++cur;
                break;
            case '\0':
            default:
                // Illegal backslash escape
                return std::nullopt;
            }
            break;
        }
        case '"':
        {
            last = cur;
            ++cur;
            is_string_end = true;
            break;
        }
        default:
            ++cur;
            break;
        }
    }
    if (cur == content.cend())
    {
        return std::nullopt;
    }

    return std::string(first, last);
}

bool json::parser::skip_whitespace(
    const std::string &content, std::string::const_iterator &cur) noexcept
{
    while (true)
    {
        switch (*cur)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            ++cur;
            break;
        case '\0': // std::string::end()
            return false;
        default:
            return true;
        }
    }
}

bool json::parser::skip_digit(
    const std::string &content, std::string::const_iterator &cur) noexcept
{
    // At least one digit
    if (isdigit(*cur))
    {
        ++cur;
    }
    else
    {
        return false;
    }

    while (isdigit(*cur))
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
