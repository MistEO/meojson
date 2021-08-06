#include "json_parser.h"

#include <cctype>

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
    return parser(content.cbegin(), content.cend()).parse();
}

std::optional<json::value> json::parser::parse()
{
    if (!skip_whitespace())
    {
        return std::nullopt;
    }

    value result_value;
    switch (*_cur)
    {
    case '[':
        result_value = parse_array();
        break;
    case '{':
        result_value = parse_object();
        break;
    default: // A JSON payload should be an object or array
        return std::nullopt;
    }

    if (!result_value.valid())
    {
        return std::nullopt;
    }

    // After the parsing is complete, there should be no more content other than spaces behind
    if (skip_whitespace())
    {
        return std::nullopt;
    }

    return result_value;
}

json::value json::parser::parse_value()
{
    switch (*_cur)
    {
    case 'n':
        return parse_null();
    case 't':
    case 'f':
        return parse_boolean();
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
        return parse_number();
    case '"':
        return parse_string();
    case '[':
        return parse_array();
    case '{':
        return parse_object();
    default:
        return invalid_value();
    }
}

json::value json::parser::parse_null()
{
    static const std::string null_string = "null";

    for (auto &&ch : null_string)
    {
        if (*_cur == ch)
        {
            ++_cur;
        }
        else
        {
            return invalid_value();
        }
    }

    return value();
}

json::value json::parser::parse_boolean()
{
    static const std::string true_string = "true";
    static const std::string false_string = "false";

    switch (*_cur)
    {
    case 't':
        for (auto &&ch : true_string)
        {
            if (*_cur == ch)
            {
                ++_cur;
            }
            else
            {
                return invalid_value();
            }
        }
        return true;
    case 'f':
        for (auto &&ch : false_string)
        {
            if (*_cur == ch)
            {
                ++_cur;
            }
            else
            {
                return invalid_value();
            }
        }
        return false;
    default:
        return invalid_value();
    }
}

json::value json::parser::parse_number()
{
    const auto first = _cur;
    if (*_cur == '-')
    {
        ++_cur;
    }

    // Numbers cannot have leading zeroes
    if (_cur != _end && *_cur == '0' &&
        _cur + 1 != _end && std::isdigit(*(_cur + 1)))
    {
        return invalid_value();
    }

    if (!skip_digit())
    {
        return invalid_value();
    }

    if (*_cur == '.')
    {
        ++_cur;
        if (!skip_digit())
        {
            return invalid_value();
        }
    }

    if (*_cur == 'e' || *_cur == 'E')
    {
        if (++_cur == _end)
        {
            return invalid_value();
        }
        if (*_cur == '+' || *_cur == '-')
        {
            ++_cur;
        }
        if (!skip_digit())
        {
            return invalid_value();
        }
    }

    return value(value_type::Number, first, _cur);
}

json::value json::parser::parse_string()
{
    auto string_opt = parse_stdstring();
    if (!string_opt)
    {
        return invalid_value();
    }

    return std::move(string_opt).value();
}

json::value json::parser::parse_array()
{
    if (*_cur == '[')
    {
        ++_cur;
    }
    else
    {
        return invalid_value();
    }

    if (!skip_whitespace())
    {
        return invalid_value();
    }
    else if (*_cur == ']')
    {
        ++_cur;
        // empty array
        return array();
    }

    array result;
    while (true)
    {
        if (!skip_whitespace())
        {
            return invalid_value();
        }

        auto val = parse_value();

        if (!val.valid() || !skip_whitespace())
        {
            return invalid_value();
        }

        result.emplace_back(std::move(val));

        if (*_cur == ',')
        {
            ++_cur;
        }
        else
        {
            break;
        }
    }

    if (skip_whitespace() && *_cur == ']')
    {
        ++_cur;
    }
    else
    {
        return invalid_value();
    }

    return result;
}

json::value json::parser::parse_object()
{
    if (*_cur == '{')
    {
        ++_cur;
    }
    else
    {
        return invalid_value();
    }

    if (!skip_whitespace())
    {
        return invalid_value();
    }
    else if (*_cur == '}')
    {
        ++_cur;
        // empty object
        return object();
    }

    object result;
    while (true)
    {
        if (!skip_whitespace())
        {
            return invalid_value();
        }

        auto key_opt = parse_stdstring();

        if (key_opt && skip_whitespace() && *_cur == ':')
        {
            ++_cur;
        }
        else
        {
            return invalid_value();
        }

        if (!skip_whitespace())
        {
            return invalid_value();
        }

        auto val = parse_value();

        if (!val.valid() || !skip_whitespace())
        {
            return invalid_value();
        }

        result.emplace(std::move(key_opt).value(), std::move(val));

        if (*_cur == ',')
        {
            ++_cur;
        }
        else
        {
            break;
        }
    }

    if (skip_whitespace() && *_cur == '}')
    {
        ++_cur;
    }
    else
    {
        return invalid_value();
    }

    return result;
}

std::optional<std::string> json::parser::parse_stdstring()
{
    if (*_cur == '"')
    {
        ++_cur;
    }
    else
    {
        return std::nullopt;
    }

    const auto first = _cur;
    auto last = _cur;
    bool is_string_end = false;
    while (!is_string_end && _cur != _end)
    {
        switch (*_cur)
        {
        case '\t':
        case '\r':
        case '\n':
            return std::nullopt;
        case '\\':
        {
            if (++_cur == _end)
            {
                return std::nullopt;
            }
            switch (*_cur)
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
                ++_cur;
                break;
            default:
                // Illegal backslash escape
                return std::nullopt;
            }
            break;
        }
        case '"':
        {
            last = _cur;
            ++_cur;
            is_string_end = true;
            break;
        }
        default:
            ++_cur;
            break;
        }
    }
    if (_cur == _end)
    {
        return std::nullopt;
    }

    return std::string(first, last);
}

bool json::parser::skip_whitespace() noexcept
{
    while (_cur != _end)
    {
        switch (*_cur)
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            ++_cur;
            break;
        default:
            return true;
        }
    }
    return false;
}

bool json::parser::skip_digit() noexcept(noexcept(std::isdigit(*_cur)))
{
    // At least one digit
    if (_cur != _end && std::isdigit(*_cur))
    {
        ++_cur;
    }
    else
    {
        return false;
    }

    while (_cur != _end && std::isdigit(*_cur))
    {
        ++_cur;
    }

    if (_cur != _end)
    {
        return true;
    }
    else
    {
        return false;
    }
}
