#include "json_value.h"
#include "json_object.h"
#include "json_array.h"
#include "json_parser.h"
#include "json_exception.h"

// for Pimpl
json::value::value() = default;

json::value::value(const json::value &rhs)
    : _type(rhs._type),
      _raw_data(rhs._raw_data),
      _lazy_data(rhs._lazy_data),
      _array_ptr(rhs._array_ptr == nullptr ? nullptr : std::make_unique<array>(*rhs._array_ptr)),
      _object_ptr(rhs._object_ptr == nullptr ? nullptr : std::make_unique<object>(*rhs._object_ptr))
{
    ;
}

json::value::value(json::value &&rhs) noexcept
    : _type(std::move(rhs._type)),
      _raw_data(std::move(rhs._raw_data)),
      _lazy_data(std::move(rhs._lazy_data)),
      _array_ptr(std::move(rhs._array_ptr)),
      _object_ptr(std::move(rhs._object_ptr))
{
    rhs._type = value_type::Invalid;
}

json::value::value(bool b)
    : _type(value_type::Boolean),
      _raw_data(b ? "true" : "false")
{
    ;
}

json::value::value(int num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(unsigned num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(long num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(unsigned long num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(long long num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(unsigned long long num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(float num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(double num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(long double num)
    : _type(value_type::Number),
      _raw_data(std::to_string(num))
{
    ;
}

json::value::value(const char *str)
    : _type(value_type::String),
      _raw_data(str)
{
    ;
}

json::value::value(const std::string &str)
    : _type(value_type::String),
      _raw_data(str)
{
    ;
}

json::value::value(std::string &&str)
    : _type(value_type::String),
      _raw_data(std::move(str))
{
    ;
}

json::value::value(const array &arr)
    : _type(value_type::Array),
      _raw_data(std::string()),
      _array_ptr(std::make_unique<array>(arr))
{
    ;
}

json::value::value(array &&arr)
    : _type(value_type::Array),
      _raw_data(std::string()),
      _array_ptr(std::make_unique<array>(std::move(arr)))
{
    ;
}

json::value::value(const object &obj)
    : _type(value_type::Object),
      _raw_data(std::string()),
      _object_ptr(std::make_unique<object>(obj))
{
    ;
}

json::value::value(object &&obj)
    : _type(value_type::Object),
      _raw_data(std::string()),
      _object_ptr(std::make_unique<object>(std::move(obj)))
{
    ;
}

// json::value::value(std::initializer_list<value> init_list)
//     : _type(value_type::Array),
//       _raw_data(std::string()),
//       _array_ptr(std::make_unique<array>(init_list))
// {
//     ;
// }

// json::value::value(std::initializer_list<std::pair<std::string, value>> init_list)
//     : _type(value_type::Object),
//       _raw_data(std::string()),
//       _object_ptr(std::make_unique<object>(init_list))
// {
//     ;
// }

// json::value::value(json::value_type type, std::string &&raw_data)
//     : _type(type),
//       _raw_data((type == value_type::Array || type == value_type::Object) ? std::string() : std::move(raw_data)),
//       _lazy_data((type == value_type::Array || type == value_type::Object) ? std::move(raw_data) : std::string())
// {
//     ;
// }

// json::value::value(unique_array &&arr_ptr)
//     : _type(value_type::Array),
//       _array_ptr(std::move(arr_ptr))
// {
//     ;
// }

// json::value::value(unique_object &&obj_ptr)
//     : _type(value_type::Object),
//       _object_ptr(std::move(obj_ptr))
// {
//     ;
// }

// bool json::value::valid() const noexcept
// {
//     return _type != value_type::Invalid ? true : false;
// }

// bool json::value::empty() const noexcept
// {
//     return (_type == value_type::Null && _raw_data.compare("null") == 0) ? true : false;
// }

// json::value_type json::value::type() const noexcept
// {
//     return _type;
// }

// for Pimpl
json::value::~value() = default;

const json::value &json::value::at(size_t pos) const
{
    if (_type == value_type::Array && _array_ptr != nullptr)
    {
        return _array_ptr->at(pos);
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Array && !_lazy_data.empty())
    {
        parse_lazy_data();
        return _array_ptr->at(pos);
    }
    lazy_lock.unlock();

    throw exception("Wrong Type or data empty");
}

const json::value &json::value::at(const std::string &key) const
{
    if (_type == value_type::Object && _object_ptr != nullptr)
    {
        return _object_ptr->at(key);
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Object && !_lazy_data.empty())
    {
        parse_lazy_data();
        return _object_ptr->at(key);
    }
    lazy_lock.unlock();

    throw exception("Wrong Type or data empty");
}

bool json::value::as_boolean() const
{
    if (_type == value_type::Boolean)
    {
        if (_raw_data == "true")
        {
            return true;
        }
        else if (_raw_data == "false")
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
        return std::stoi(_raw_data);
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
//         return std::stou(_raw_data); // not exist
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
        return std::stol(_raw_data);
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
        return std::stoul(_raw_data);
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
        return std::stoll(_raw_data);
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
        return std::stoull(_raw_data);
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
        return std::stof(_raw_data);
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
        return std::stod(_raw_data);
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
        return std::stold(_raw_data);
    }
    else
    {
        throw exception("Wrong Type");
    }
}

std::string json::value::as_string() const
{
    if (_type == value_type::String)
    {
        return _raw_data;
    }
    else
    {
        throw exception("Wrong Type");
    }
}

json::array json::value::as_array() const
{
    if (_type == value_type::Array && _array_ptr != nullptr)
    {
        return *_array_ptr;
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Array && !_lazy_data.empty())
    {
        parse_lazy_data();
        return *_array_ptr;
    }
    lazy_lock.unlock();

    throw exception("Wrong Type");
}

json::object json::value::as_object() const
{
    if (_type == value_type::Object && _object_ptr != nullptr)
    {
        return *_object_ptr;
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Object && !_lazy_data.empty())
    {
        parse_lazy_data();
        return *_object_ptr;
    }

    throw exception("Wrong Type or data empty");
}

std::string json::value::to_string() const
{
    switch (_type)
    {
    case value_type::Null:
    case value_type::Boolean:
    case value_type::Number:
        return _raw_data;
    case value_type::String:
        return '"' + _raw_data + '"';
    case value_type::Object:
        if (_object_ptr == nullptr)
        {
            std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
            if (!_lazy_data.empty())
            {
                parse_lazy_data();
            }
            else
            {
                throw exception("Object data error");
            }
        }
        return _object_ptr->to_string();
    case value_type::Array:
        if (_array_ptr == nullptr)
        {
            std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
            if (!_lazy_data.empty())
            {
                parse_lazy_data();
            }
            else
            {
                throw exception("Array data error");
            }
        }
        return _array_ptr->to_string();
    default:
        throw exception("Unknown Value Type");
    }
}

std::string json::value::format(std::string shift_str, size_t basic_shift_count) const
{
    switch (_type)
    {
    case value_type::Null:
    case value_type::Boolean:
    case value_type::Number:
        return _raw_data;
    case value_type::String:
        return '"' + _raw_data + '"';
    case value_type::Object:
        if (_object_ptr == nullptr)
        {
            std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
            if (!_lazy_data.empty())
            {
                parse_lazy_data();
            }
            else
            {
                throw exception("Object data error");
            }
        }
        return _object_ptr->format(shift_str, basic_shift_count);
    case value_type::Array:
        if (_array_ptr == nullptr)
        {
            std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
            if (!_lazy_data.empty())
            {
                parse_lazy_data();
            }
            else
            {
                throw exception("Array data error");
            }
        }
        return _array_ptr->format(shift_str, basic_shift_count);
    default:
        throw exception("Unknown Value Type");
    }
}

json::value &json::value::operator=(const value &rhs)
{
    _type = rhs._type;
    _raw_data = rhs._raw_data;
    _lazy_data = rhs._lazy_data;
    // _lazy_mutex;
    _array_ptr = rhs._array_ptr == nullptr ? nullptr : std::make_unique<array>(*rhs._array_ptr);
    _object_ptr = rhs._object_ptr == nullptr ? nullptr : std::make_unique<object>(*rhs._object_ptr);

    return *this;
}

json::value &json::value::operator=(value &&rhs) noexcept
{
    _type = std::move(rhs._type);
    rhs._type = value_type::Invalid;

    _raw_data = std::move(rhs._raw_data);
    _lazy_data = std::move(rhs._lazy_data);
    // _lazy_mutex;
    _array_ptr = std::move(rhs._array_ptr);
    _object_ptr = std::move(rhs._object_ptr);

    return *this;
}

const json::value &json::value::operator[](size_t pos) const
{
    if (_type == value_type::Array && _array_ptr != nullptr)
    {
        return (*_array_ptr)[pos];
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Array && !_lazy_data.empty())
    {
        parse_lazy_data();
        return (*_array_ptr)[pos];
    }
    lazy_lock.unlock();
    // Array not support to create by operator[]

    throw exception("Wrong Type");
}

json::value &json::value::operator[](size_t pos)
{
    if (_type == value_type::Array && _array_ptr != nullptr)
    {
        return (*_array_ptr)[pos];
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Array && !_lazy_data.empty())
    {
        parse_lazy_data();
        return (*_array_ptr)[pos];
    }
    lazy_lock.unlock();
    // Array not support to create by operator[]

    throw exception("Wrong Type");
}

json::value &json::value::operator[](const std::string &key)
{
    if (_type == value_type::Object && _object_ptr != nullptr)
    {
        return (*_object_ptr)[key];
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Object && !_lazy_data.empty())
    {
        parse_lazy_data();
        return (*_object_ptr)[key];
    }
    // Create a new value by operator[]
    else if (_type == value_type::Null)
    {
        _type = value_type::Object;
        _object_ptr = std::make_unique<object>();
        return (*_object_ptr)[key];
    }
    lazy_lock.unlock();

    throw exception("Wrong Type");
}

json::value &json::value::operator[](std::string &&key)
{
    if (_type == value_type::Object && _object_ptr != nullptr)
    {
        return (*_object_ptr)[std::move(key)];
    }

    std::unique_lock<std::mutex> lazy_lock(_lazy_mutex);
    if (_type == value_type::Object && !_lazy_data.empty())
    {
        parse_lazy_data();
        return (*_object_ptr)[std::move(key)];
    }
    // Create a new value by operator[]
    else if (_type == value_type::Null)
    {
        _type = value_type::Object;
        _object_ptr = std::make_unique<object>();
        return (*_object_ptr)[std::move(key)];
    }
    lazy_lock.unlock();

    throw exception("Wrong Type");
}

void json::value::parse_lazy_data() const
{
    auto value_opt = json::parser::parse(_lazy_data, 1);

    if (value_opt)
    {
        if (_type == value_type::Array)
        {
            _array_ptr = std::move(value_opt).value()._array_ptr;
            _lazy_data.clear();
        }
        else if (_type == value_type::Object)
        {
            _object_ptr = std::move(value_opt).value()._object_ptr;
            _lazy_data.clear();
        }
        else
        {
            throw exception("Wrong internal call");
        }
    }
    else
    {
        throw exception("Raw data error");
    }
}

json::value json::value::invalid_value()
{
    return value(value_type::Invalid, std::string());
}

std::ostream &operator<<(std::ostream &out, const json::value &val)
{
    // TODO: format output

    out << val.to_string();
    return out;
}