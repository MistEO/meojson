#include "json_value.h"

#include <regex>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#include "json_exception.h"

// Reference
// Json     https://www.json.org/json-zh.html
// Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
const std::string json::value::reg_str_json_whitespace("(?:\\s*)");

const std::string json::value::reg_str_json_null("(null)");

const std::string json::value::reg_str_json_boolean("(true|false)");

const std::string json::value::reg_str_json_string("(\"[^\"]*\")");

const std::string json::value::reg_str_json_number_fraction("(?:\\.\\d+)?");
const std::string json::value::reg_str_json_number_exponent("(?:(?:e|E)(?:-|\\+)?\\d+)?");
const std::string json::value::reg_str_json_number("(-?\\d+" + json::value::reg_str_json_number_fraction + json::value::reg_str_json_number_exponent + ")");

const std::string json::value::reg_str_json_non_nested_value("(?:" + json::value::reg_str_json_null + "|" + json::value::reg_str_json_boolean + "|" + json::value::reg_str_json_string + "|" + json::value::reg_str_json_number + ")");

const std::regex json::value::reg_json_null("^" + json::value::reg_str_json_null);
const std::regex json::value::reg_json_boolean("^" + json::value::reg_str_json_boolean);
const std::regex json::value::reg_json_string("^" + json::value::reg_str_json_string);
const std::regex json::value::reg_json_number("^" + json::value::reg_str_json_number);
const std::regex json::value::reg_json_non_nested_value("^" + json::value::reg_str_json_non_nested_value);

bool json::value::parse(const std::string &content)
{
    auto parse_iter = content.cbegin();
    parse_value(content, parse_iter);
}

std::string::const_iterator json::value::parse_value(const std::string &content, const std::string::const_iterator &first)
{
    auto cur = first;
    if (cur == content.cend())
    {
        return content.cend();
    }

    // 去空格
    auto remove_white_space_func = [&]() -> decltype(cur) {
        while (*cur == ' ' || *cur == '\t' || *cur == '\r' || *cur == '\n')
        {
            ++cur;
            if (cur == content.cend())
            {
                return content.cend();
            }
        }
        return cur;
    };

    if (remove_white_space_func() == content.cend())
    {
        return content.cend();
    }

    std::string cur_string(cur, content.cend());
    switch (*cur)
    {
    case '{':
        break;
    case '[':
        break;
    case '"':
    {
        std::smatch match_result;
        if (std::regex_search(cur_string, match_result, reg_json_string))
        {
            if (match_result.size() == 2)
            {
                std::string val = match_result[1];
                cur += val.size();
            }
        }
        break;
    }
    case 'n':
        break;
    case 't':
    case 'f':
        break;
    default:
        break;
    }
    return cur;
}

bool json::value::valid() const
{
    return m_type != JsonInvalid;
}

json::value::ValueType json::value::type() const
{
    return m_type;
}

bool json::value::as_boolean() const
{
    if (m_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (m_type == JsonBoolean)
    {
        if (m_raw == "true")
        {
            return true;
        }
        else if (m_raw == "false")
        {
            return false;
        }
        else
        {
            throw json::exception("Unknown Parse Error");
        }
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

int json::value::as_integer() const
{
    if (m_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (m_type == JsonNumber)
    {
        return std::stoi(m_raw);
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

double json::value::as_double() const
{
    if (m_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (m_type == JsonNumber)
    {
        return std::stod(m_raw);
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

std::string json::value::as_string() const
{
    if (m_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (m_type == JsonString)
    {
        std::string str = m_raw.substr(1, m_raw.size() - 2);
        return str;
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

// json::object json::value::as_object() const
// {
//     if (m_type == JsonInvalid)
//     {
//         throw json::exception("Invalid json");
//     }
//     else if (m_type == JsonObject)
//     {

//         json::object object;
//         if (object.parse(m_raw))
//         {
//             return object;
//         }
//         else
//         {
//             throw json::exception("Unknown Parse Error");
//         }
//     }
//     else
//     {
//         throw json::exception("Wrong Type");
//     }
// }

// json::array json::value::as_array() const
// {
//     if (m_type == JsonInvalid)
//     {
//         throw json::exception("Invalid json");
//     }
//     else if (m_type == JsonArray)
//     {

//         json::array array;
//         if (array.parse(m_raw))
//         {
//             return array;
//         }
//         else
//         {
//             throw json::exception("Unknown Parse Error");
//         }
//     }
//     else
//     {
//         throw json::exception("Wrong Type");
//     }
// }

std::string json::value::to_string() const
{
    return m_raw;
}

json::value json::value::string(const char *str)
{
    json::value val;
    val.m_raw = std::string() + "\"" + str + "\"";
    val.m_type = JsonString;
    return val;
}

json::value json::value::string(const std::string &str)
{
    json::value val;
    val.m_raw = "\"" + str + "\"";
    val.m_type = JsonString;
    return val;
}

json::value json::value::number(int num)
{
    json::value val;
    val.m_raw = std::to_string(num);
    val.m_type = JsonNumber;
    return val;
}

json::value json::value::number(double num)
{
    json::value val;
    val.m_raw = std::to_string(num);
    val.m_type = JsonNumber;
    return val;
}

json::value json::value::boolean(bool b)
{
    json::value val;
    val.m_raw = b ? "true" : "false";
    val.m_type = JsonBoolean;
    return val;
}

json::value json::value::null()
{
    json::value val;
    val.m_raw = "null";
    val.m_type = JsonNull;
    return val;
}

// json::value json::value::object(const json::object &obj)
// {
//     json::value val;
//     val.m_raw = obj.to_string();
//     return val;
// }

// json::value json::value::array(const json::array &arr)
// {
//     json::value val;
//     val.m_raw = arr.to_string();
//     return val;
// }
