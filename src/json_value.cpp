#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"

json::value::value(bool b)
    : _type(value_type::Boolean),
      _raw_basic_data(b ? "true" : "false")
{
    ;
}

json::value::value(int num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(unsigned num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(long num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(unsigned long num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(long long num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(unsigned long long num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(float num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(double num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(long double num)
    : _type(value_type::Number),
      _raw_basic_data(std::to_string(num))
{
    ;
}

json::value::value(const char *str)
    : _type(value_type::String),
      _raw_basic_data(std::string() + "\"" + str + "\"")
{
    ;
}

json::value::value(const std::string &str)
    : _type(value_type::String),
      _raw_basic_data("\"" + str + "\"")
{
    ;
}

json::value::value(std::string &&str)
    : _type(value_type::String),
      _raw_basic_data("\"" + std::forward<std::string>(str) + "\"")
{
    ;
}

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
    if (_type == value_type::Null && _raw_basic_data.compare("null") == 0)
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

const json::value &json::value::at(size_t pos) const
{
    if (_type == value_type::Array)
    {
        return _array_ptr->at(pos);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

const json::value &json::value::at(const std::string key) const
{
    if (_type == value_type::Object)
    {
        return _object_ptr->at(key);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

bool json::value::as_boolean() const
{
    if (_type == value_type::Boolean)
    {
        if (_raw_basic_data == "true")
        {
            return true;
        }
        else if (_raw_basic_data == "false")
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
        return std::stoi(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

// unsigned json::value::as_unsigned() const
// {
//     if (_type == value_type::Number)
//     {
//         return std::stou(_raw_basic_data); // not exist
//     }
//     else
//     {
//         throw exception("Wrong Type");
//     }
// }

long json::value::as_long() const
{
    if (_type == value_type::Number)
    {
        return std::stol(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

unsigned long json::value::as_unsigned_long() const
{
    if (_type == value_type::Number)
    {
        return std::stoul(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

long long json::value::as_long_long() const
{
    if (_type == value_type::Number)
    {
        return std::stoll(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

unsigned long long json::value::as_unsigned_long_long() const
{
    if (_type == value_type::Number)
    {
        return std::stoull(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

float json::value::as_float() const
{
    if (_type == value_type::Number)
    {
        return std::stof(_raw_basic_data);
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
        return std::stod(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

long double json::value::as_long_double() const
{
    if (_type == value_type::Number)
    {
        return std::stold(_raw_basic_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

std::string json::value::as_string() const
{
    if (_type == value_type::String &&
        _raw_basic_data.size() >= 2 &&
        _raw_basic_data.at(0) == '"' &&
        _raw_basic_data.at(_raw_basic_data.size() - 1) == '"')
    {
        return _raw_basic_data.substr(1, _raw_basic_data.size() - 2);
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
        return _raw_basic_data;
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
    _raw_basic_data = basic_data;
}

void json::value::set_raw_basic_data(json::value_type type, std::string &&basic_data)
{
    _type = type;
    _raw_basic_data = std::forward<std::string>(basic_data);
}

const json::value &json::value::operator[](size_t pos) const
{
    if (_type == value_type::Array)
    {
        return (*_array_ptr)[pos];
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::value &json::value::operator[](size_t pos)
{
    if (_type == value_type::Array)
    {
        return (*_array_ptr)[pos];
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::value &json::value::operator[](const std::string &key)
{
    if (_type == value_type::Object)
    {
        return (*_object_ptr)[key];
    }
    else if (_type == value_type::Null)
    {
        _type = value_type::Object;
        _object_ptr = std::make_shared<object>();
        return (*_object_ptr)[key];
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::value &json::value::operator[](std::string &&key)
{
    if (_type == value_type::Object)
    {
        return (*_object_ptr)[std::forward<std::string>(key)];
    }
    else if (_type == value_type::Null)
    {
        _type = value_type::Object;
        _object_ptr = std::make_shared<object>();
        return (*_object_ptr)[std::forward<std::string>(key)];
    }
    else
    {
        throw exception("Wrong Type");
    }
}

std::ostream &operator<<(std::ostream &out, const json::value &val)
{
    // TODO: format output

    out << val.to_string();
    return out;
}