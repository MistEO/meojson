#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

json::value::value(const object &obj)
    : _type(JsonObject),
      _object_data(obj.raw_data())
{
    ;
}

json::value::value(const array &arr)
    : _type(JsonArray),
      _array_data(arr.raw_data())
{
}

bool json::value::empty() const
{
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonWhiteSpace)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool json::value::valid() const
{
    if (_type != JsonInvalid)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool json::value::as_boolean() const
{
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonBoolean)
    {
        if (_basic_type_data == "true")
        {
            return true;
        }
        else if (_basic_type_data == "false")
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
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonNumber)
    {
        return std::stoi(_basic_type_data);
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

double json::value::as_double() const
{
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonNumber)
    {
        return std::stod(_basic_type_data);
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

std::string json::value::as_string() const
{
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonString)
    {
        std::string str = _basic_type_data.substr(1, _basic_type_data.size() - 2);
        return str;
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

json::object json::value::as_object() const
{
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonObject)
    {
        return json::object(_object_data);
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

json::array json::value::as_array() const
{
    if (_type == JsonInvalid)
    {
        throw json::exception("Invalid json");
    }
    else if (_type == JsonArray)
    {
        return json::array(_array_data);
    }
    else
    {
        throw json::exception("Wrong Type");
    }
}

std::string json::value::to_string() const
{
    switch (_type)
    {
    case JsonInvalid:
        throw json::exception("Invalid json");
    case JsonWhiteSpace:
    case JsonNull:
    case JsonBoolean:
    case JsonString:
    case JsonNumber:
        return _basic_type_data;
    case JsonObject:
        return json::object(_object_data).to_string();
    case JsonArray:
        return json::array(_array_data).to_string();
    default:
        throw json::exception("Unknown Value Type");
    }
}

json::value json::value::string(const char *str)
{
    json::value val;
    val._type = JsonString;
    val._basic_type_data = std::string() + "\"" + str + "\"";
    return val;
}

json::value json::value::string(const std::string &str)
{
    json::value val;
    val._type = JsonString;
    val._basic_type_data = "\"" + str + "\"";
    return val;
}

json::value json::value::number(int num)
{
    json::value val;
    val._type = JsonNumber;
    val._basic_type_data = std::to_string(num);
    return val;
}

json::value json::value::number(double num)
{
    json::value val;
    val._type = JsonNumber;
    val._basic_type_data = std::to_string(num);
    return val;
}

json::value json::value::boolean(bool b)
{
    json::value val;
    val._type = JsonBoolean;
    val._basic_type_data = b ? "true" : "false";
    return val;
}

json::value json::value::null()
{
    json::value val;
    val._type = JsonNull;
    val._basic_type_data = "null";
    return val;
}

// json::value json::value::object(const json::object &obj)
// {
//     json::value val;
//     val._type = JsonObject;
//     val._object_data = obj.raw_data();
//     return val;
// }

// json::value json::value::array(const json::array &arr)
// {
//     json::value val;
//     val._type = JsonArray;
//     val._array_data = arr.raw_data();
//     return val;
// }
