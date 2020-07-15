#include "json_value.h"
#include "json_object.h"
#include "json_array.h"

#include <regex>

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
// To do
bool json::value::parse(const std::string &content)
{
    // null
    // if (content.empty() || std::regex_match(content, std::regex("\s*")))
    // {
    //     m_valid = true;
    // }

    m_raw = content;
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