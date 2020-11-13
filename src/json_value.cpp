#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

json::value::value(const array &arr)
    : _type(value_type::Array),
      _array_ptr(std::make_shared<array>(arr))
{
    ;
}

json::value::value(array &&arr)
    : _type(value_type::Array),
      _array_ptr(std::make_shared<array>(std::forward<array>(arr)))
{
    ;
}

json::value::value(const object &obj)
    : _type(value_type::Object),
      _object_ptr(std::make_shared<object>(obj))
{
    ;
}

json::value::value(object &&obj)
    : _type(value_type::Object),
      _object_ptr(std::make_shared<object>(std::forward<object>(obj)))
{
    ;
}

bool json::value::valid() const noexcept
{
    if (_type != value_type::Invalid)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool json::value::empty() const noexcept
{
    if (_type == value_type::Null && _basic_type_data.compare("null") == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

json::value_type json::value::type() const noexcept
{
    return _type;
}

bool json::value::as_boolean() const
{
    if (_type == value_type::Boolean)
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
    if (_type == value_type::Number)
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
    if (_type == value_type::Number)
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
    if (_type == value_type::String &&
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

json::array json::value::as_array() const
{
    if (_type == value_type::Array)
    {
        return *_array_ptr;
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::object json::value::as_object() const
{
    if (_type == value_type::Object)
    {
        return *_object_ptr;
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
    case value_type::Null:
    case value_type::Boolean:
    case value_type::String:
    case value_type::Number:
        return _basic_type_data;
    case value_type::Object:
        return _object_ptr->to_string();
    case value_type::Array:
        return _array_ptr->to_string();
    default:
        throw exception("Unknown Value Type");
    }
}

void json::value::set_raw_basic_data(json::value_type type, const std::string &basic_data)
{
    _type = type;
    _basic_type_data = basic_data;
}

void json::value::set_raw_basic_data(json::value_type type, std::string &&basic_data)
{
    _type = type;
    _basic_type_data = std::forward<std::string>(basic_data);
}

json::value json::value::null()
{
    return value();
}

json::value json::value::boolean(bool b)
{
    value val;
    val._type = value_type::Boolean;
    val._basic_type_data = b ? "true" : "false";
    return val;
}

json::value json::value::number(int num)
{
    value val;
    val._type = value_type::Number;
    val._basic_type_data = std::to_string(num);
    return val;
}

json::value json::value::number(double num)
{
    value val;
    val._type = value_type::Number;
    val._basic_type_data = std::to_string(num);
    return val;
}

json::value json::value::string(const char *str)
{
    value val;
    val._type = value_type::String;
    val._basic_type_data = std::string() + "\"" + str + "\"";
    return val;
}

json::value json::value::string(const std::string &str)
{
    value val;
    val._type = value_type::String;
    val._basic_type_data = "\"" + str + "\"";
    return val;
}

json::value json::value::string(std::string &&str)
{
    value val;
    val._type = value_type::String;
    val._basic_type_data = "\"" + std::forward<std::string>(str) + "\"";
    return val;
}

std::ostream &operator<<(std::ostream &out, const json::value &val)
{
    // TODO: format output

    out << val.to_string();
    return out;
}