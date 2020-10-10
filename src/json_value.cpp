#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

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

json::object json::value::as_object() const
{
    if (m_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (m_type == JsonObject)
    {

        json::object object;
        if (object.parse(m_raw))
        {
            return object;
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

json::array json::value::as_array() const
{
    if (m_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (m_type == JsonArray)
    {

        json::array array;
        if (array.parse(m_raw))
        {
            return array;
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
