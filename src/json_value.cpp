#include "json_value.h"
#include "json_object.h"
#include "json_array.h"

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

json::object json::value::as_object() const
{
    return json::object();
}

json::array json::value::as_array() const
{
    return json::array();
}

std::string json::value::to_string() const
{
    return m_raw;
}