#include "json_value.h"

#include <regex>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

json::value::value(const json::object &obj)
{
    m_raw = obj.to_string();
    m_valid = obj.valid();
}

json::value::value(const json::array &arr)
{
    m_raw = arr.to_string();
    m_valid = arr.valid();
}

bool json::value::parse(const std::string &content, bool only_judge_valid)
{
    std::string format_content(content);
    std::replace(format_content.begin(), format_content.end(), '\n', ' ');
    if (!only_judge_valid)
    {
        m_raw = format_content;
    }

    // Reference
    // Json     https://www.json.org/json-zh.html
    // Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
    static const std::string reg_str_json_whitespace = "(?:\\s*)";

    static const std::string reg_str_json_null = "(?:null)";
    static const std::string reg_str_json_boolean = "(?:true|false)";
    static const std::string reg_str_json_string = "(?:\".*?\")";

    static const std::string reg_str_json_number_fraction = "(?:\\.\\d+)?";
    static const std::string reg_str_json_number_exponent = "(?:(?:e|E)(?:-|\\+)?\\d+)?";
    static const std::string reg_str_json_number = "(?:-?\\d+" + reg_str_json_number_fraction + reg_str_json_number_exponent + ")";

    static const std::string reg_str_json_value = "(" + reg_str_json_null + "|" + reg_str_json_string + "|" + reg_str_json_number + "|(?:\\{.*?\\})|(?:\\[.*?\\]))";

    static const std::string reg_str_json_object_pair = "(?:" + reg_str_json_string + reg_str_json_whitespace + "\\: " + reg_str_json_whitespace + reg_str_json_value + ")";
    static const std::string reg_str_json_object = "\\{" + reg_str_json_whitespace + "(?:(?:" + reg_str_json_object_pair + reg_str_json_whitespace + "," + reg_str_json_whitespace + ")*?" + reg_str_json_object_pair + ")?" + reg_str_json_whitespace + "\\}";

    static const std::string reg_str_json_array_element = reg_str_json_value;
    static const std::string reg_str_json_array = "\\[" + reg_str_json_whitespace + "(?:(?:" + reg_str_json_array_element + reg_str_json_whitespace + "," + reg_str_json_whitespace + ")*?" + reg_str_json_array_element + ")?" + reg_str_json_whitespace + "\\]";

    static const std::regex reg_json_null(reg_str_json_null);
    static const std::regex reg_json_boolean(reg_str_json_boolean);
    static const std::regex reg_json_string(reg_str_json_string);
    static const std::regex reg_json_number(reg_str_json_number);
    static const std::regex reg_json_object(reg_str_json_object);
    static const std::regex reg_json_array(reg_str_json_array);
    static const std::regex reg_json_value(reg_str_json_value);

    if (std::regex_match(format_content, reg_json_value))
    { //just for debug
        m_valid = true;
    }
    else
    {
        m_valid = false;
        return m_valid;
    }

    std::smatch match_result;
    if (std::regex_match(format_content, reg_json_null))
    {
        m_valid = true;
        m_type = JsonNull;
    }
    else if (std::regex_match(format_content, reg_json_boolean))
    {
        m_valid = true;
        m_type = JsonBoolean;
    }
    else if (std::regex_match(format_content, reg_json_string))
    {
        m_valid = true;
        m_type = JsonString;
    }
    else if (std::regex_match(format_content, reg_json_number))
    {
        m_valid = true;
        m_type = JsonNumber;
    }
    else if (std::regex_match(format_content, match_result, reg_json_object))
    {
        for (auto it = match_result.begin() + 1; it != match_result.end(); ++it)
        {
            bool sub_parse = parse(*it, true);
            if (sub_parse == false)
            {
                m_valid = false;
                m_type = JsonInvalid;
                break;
            }
        }
        m_valid = true;
        m_type = JsonObject;
    }
    else if (std::regex_match(format_content, match_result, reg_json_array))
    {
        for (auto it = match_result.begin() + 1; it != match_result.end(); ++it)
        {
            if (parse(*it, true) == false)
            {
                m_valid = false;
                m_type = JsonInvalid;
                break;
            }
        }
        m_valid = true;
        m_type = JsonArray;
    }
    else
    {
        m_valid = false;
        m_type = JsonInvalid;
    }

    return m_valid;
}

bool json::value::valid() const
{
    return m_valid;
}

int json::value::as_integer() const
{
    return std::stoi(m_raw);
}

bool json::value::as_boolean() const
{
    if (m_type == JsonBoolean)
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
            throw json::exception("Invalid translate");
        }
    }
    else
    {
        throw json::exception("Invalid translate");
    }
}

double json::value::as_double() const
{
    return std::stod(m_raw);
}

std::string json::value::as_string() const
{
    return m_raw;
}

json::object json::value::as_object() const
{
    json::object object;
    object.parse(m_raw);
    return object;
}
// To do
json::array json::value::as_array() const
{
    return json::array();
}

std::string json::value::to_string() const
{
    return m_raw;
}

json::value json::value::string(const char *str)
{
    json::value val;
    val.m_raw = std::string() + "\"" + str + "\"";
    return val;
}

json::value json::value::string(const std::string &str)
{
    json::value val;
    val.m_raw = "\"" + str + "\"";
    return val;
}

json::value json::value::number(int num)
{
    json::value val;
    val.m_raw = std::to_string(num);
    return val;
}

json::value json::value::number(double num)
{
    json::value val;
    val.m_raw = std::to_string(num);
    return val;
}

json::value json::value::boolean(bool b)
{
    json::value val;
    val.m_raw = b ? "true" : "false";
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

json::value json::value::null()
{
    json::value val;
    val.m_raw = "null";
    return val;
}