#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

json::value::value(const object &obj)
    : _type(ValueType::Object),
      _object_data(obj._object_data)
{
    ;
}

json::value::value(object &&obj)
    : _type(ValueType::Object),
      _object_data(std::forward<std::map<std::string, value>>(obj._object_data))
{
    ;
}

json::value::value(const array &arr)
    : _type(ValueType::Array),
      _array_data(arr._array_data)
{
    ;
}

json::value::value(array &&arr)
    : _type(ValueType::Array),
      _array_data(std::forward<std::vector<value>>(arr._array_data))
{
    ;
}

bool json::value::empty() const
{
    if (_type == ValueType::WhiteSpace)
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
    if (_type == ValueType::Boolean)
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
            throw exception("Unknown Parse Error");
        }
    }
    else
    {
        throw exception("Wrong Type");
    }
}

int json::value::as_integer() const
{
    if (_type == ValueType::Number)
    {
        return std::stoi(_basic_type_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

double json::value::as_double() const
{
    if (_type == ValueType::Number)
    {
        return std::stod(_basic_type_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

std::string json::value::as_string() const
{
    if (_type == ValueType::String &&
        _basic_type_data.size() >= 2 &&
        _basic_type_data.at(0) == '"' &&
        _basic_type_data.at(_basic_type_data.size() - 1) == '"')
    {
        return _basic_type_data.substr(1, _basic_type_data.size() - 2);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::object json::value::as_object() const
{
    if (_type == ValueType::Object)
    {
        return object(_object_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::array json::value::as_array() const
{
    if (_type == ValueType::Array)
    {
        return array(_array_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

std::string json::value::to_string() const
{
    switch (_type)
    {
    case ValueType::WhiteSpace:
    case ValueType::Null:
    case ValueType::Boolean:
    case ValueType::String:
    case ValueType::Number:
        return _basic_type_data;
    case ValueType::Object:
        return object(_object_data).to_string();
    case ValueType::Array:
        return array(_array_data).to_string();
    default:
        throw exception("Unknown Value Type");
    }
}

json::value json::value::string(const char *str)
{
    value val;
    val._type = ValueType::String;
    val._basic_type_data = std::string() + "\"" + str + "\"";
    return val;
}

json::value json::value::string(const std::string &str)
{
    value val;
    val._type = ValueType::String;
    val._basic_type_data = "\"" + str + "\"";
    return val;
}

json::value json::value::number(int num)
{
    value val;
    val._type = ValueType::Number;
    val._basic_type_data = std::to_string(num);
    return val;
}

json::value json::value::number(double num)
{
    value val;
    val._type = ValueType::Number;
    val._basic_type_data = std::to_string(num);
    return val;
}

json::value json::value::boolean(bool b)
{
    value val;
    val._type = ValueType::Boolean;
    val._basic_type_data = b ? "true" : "false";
    return val;
}

json::value json::value::null()
{
    value val;
    val._type = ValueType::Null;
    val._basic_type_data = "null";
    return val;
}

void json::value::set_raw_basic_data(json::ValueType type, const std::string &basic_data)
{
    _type = type;
    _basic_type_data = basic_data;
}

void json::value::set_raw_basic_data(json::ValueType type, std::string &&basic_data)
{
    _type = type;
    _basic_type_data = std::forward<std::string>(basic_data);
}

// json::value json::value::object(const json::object &obj)
// {
//     json::value val;
//     val._type = ValueType::Object;
//     val._object_data = obj.raw_data();
//     return val;
// }

// json::value json::value::array(const json::array &arr)
// {
//     json::value val;
//     val._type = ValueType::Array;
//     val._array_data = arr.raw_data();
//     return val;
// }
