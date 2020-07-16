#include "json_value.h"
#include "json_object.h"
#include "json_array.h"

#include <regex>
#include <iostream>

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

// Reference
// Json     https://www.json.org/json-zh.html
// Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
bool json::value::parse(const std::string &content)
{
    m_raw = content;

    const std::string reg_json_whitespace = "(?:\\s*)";

    const std::string reg_json_string = "(?:\".*\")";

    const std::string reg_json_number_fraction = "(?:\\.\\d+)?";
    const std::string reg_json_number_exponent = "(?:(?:e|E)(?:-|\\+)?\\d+)?";
    const std::string reg_json_number = "(?:-?\\d+" + reg_json_number_fraction + reg_json_number_exponent + ")";

    const std::string reg_json_object_pair = "(?:" + reg_json_string + reg_json_whitespace + "\\: " + reg_json_whitespace + "(.+?))";
    const std::string reg_json_object = "\\{" + reg_json_whitespace + "(?:(?:" + reg_json_object_pair + reg_json_whitespace + "," + reg_json_whitespace + ")*" + reg_json_object_pair + ")?" + reg_json_whitespace + "\\}";

    const std::string reg_json_array_element = "(.+?)";
    const std::string reg_json_array = "\\[" + reg_json_whitespace + "(?:(?:" + reg_json_array_element + reg_json_whitespace + "," + reg_json_whitespace + ")*" + reg_json_array_element + ")?" + reg_json_whitespace + "\\]";

    std::smatch match_result;
    // json value null
    if (std::regex_match(content, match_result, std::regex(reg_json_whitespace)))
    {
        m_valid = true;
        m_type = JsonNull;
    }
    // json value string
    else if (std::regex_match(content, match_result, std::regex(reg_json_string)))
    {
        m_valid = true;
        m_type = JsonString;
    }
    // json value number
    else if (std::regex_match(content, match_result, std::regex(reg_json_number)))
    {
        m_valid = true;
        m_type = JsonNumber;
    }
    // json value object
    else if (std::regex_match(content, match_result, std::regex(reg_json_object)))
    {
        m_valid = true;
        m_type = JsonObject;
        for (auto it = match_result.begin() + 1; it != match_result.end(); ++it)
        {
            bool sub_parse = parse(*it);
            if (sub_parse == false)
            {
                m_valid = false;
                m_type = JsonInvalid;
                break;
            }
        }
    }
    // json value array
    else if (std::regex_match(content, match_result, std::regex(reg_json_array)))
    {
        m_valid = true;
        m_type = JsonArray;
        for (auto it = match_result.begin() + 1; it != match_result.end(); ++it)
        {
            if (parse(*it) == false)
            {
                m_valid = false;
                m_type = JsonInvalid;
                break;
            }
        }
    }
    // invalid
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

int json::value::as_int() const
{
    return std::stoi(m_raw);
}

double json::value::as_double() const
{
    return std::stod(m_raw);
}

std::string json::value::as_string() const
{
    return m_raw;
}
// To do
json::object json::value::as_object() const
{
    return json::object();
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
    return json::value();
}