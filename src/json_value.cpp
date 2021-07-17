#include "json_value.h"

#include <mutex>

#include "json_object.h"
#include "json_array.h"
#include "json_exception.h"
// #include "json_parser.h"

// for Pimpl
json::value::value() = default;

json::value::value(const json::value & rhs)
	: _type(rhs._type),
	_raw_data(rhs._raw_data),
	_array_ptr(copy_unique_ptr(rhs._array_ptr)),
	_object_ptr(copy_unique_ptr(rhs._object_ptr))
{
	;
}

json::value::value(json::value && rhs) noexcept = default;

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

json::value::value(const char* str)
	: _type(value_type::String),
	_raw_data(str)
{
	;
}

json::value::value(const std::string & str)
	: _type(value_type::String),
	_raw_data(str)
{
	;
}

json::value::value(std::string && str)
	: _type(value_type::String),
	_raw_data(std::move(str))
{
	;
}

json::value::value(const array & arr)
	: _type(value_type::Array),
	_raw_data(std::string()),
	_array_ptr(std::make_unique<array>(arr))
{
	;
}

json::value::value(array && arr)
	: _type(value_type::Array),
	_raw_data(std::string()),
	_array_ptr(std::make_unique<array>(std::move(arr)))
{
	;
}

json::value::value(const object & obj)
	: _type(value_type::Object),
	_raw_data(std::string()),
	_object_ptr(std::make_unique<object>(obj))
{
	;
}

json::value::value(object && obj)
	: _type(value_type::Object),
	_raw_data(std::string()),
	_object_ptr(std::make_unique<object>(std::move(obj)))
{
	;
}

// for Pimpl
json::value::~value() = default;

bool json::value::exist(const std::string & key) const
{
	return _type == value_type::Object && as_object().exist(key);
}

bool json::value::exist(int index) const
{
	return _type == value_type::Array && as_array().size() < index;
}

const json::value& json::value::at(size_t pos) const
{
	if (_type == value_type::Array && _array_ptr != nullptr)
	{
		return _array_ptr->at(pos);
	}

	throw exception("Wrong Type or data empty");
}

const json::value& json::value::at(const std::string & key) const
{
	if (_type == value_type::Object && _object_ptr != nullptr)
	{
		return _object_ptr->at(key);
	}

	throw exception("Wrong Type or data empty");
}

const bool json::value::as_boolean() const
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

const int json::value::as_integer() const
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

// const unsigned json::value::as_unsigned() const
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

const long json::value::as_long() const
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

const unsigned long json::value::as_unsigned_long() const
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

const long long json::value::as_long_long() const
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

const unsigned long long json::value::as_unsigned_long_long() const
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

const float json::value::as_float() const
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

const double json::value::as_double() const
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

const long double json::value::as_long_double() const
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

const std::string json::value::as_string() const
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

const json::array json::value::as_array() const
{
	if (_type == value_type::Array && _array_ptr != nullptr)
	{
		return *_array_ptr;
	}

	throw exception("Wrong Type");
}

const json::object json::value::as_object() const
{
	if (_type == value_type::Object && _object_ptr != nullptr)
	{
		return *_object_ptr;
	}

	throw exception("Wrong Type or data empty");
}

const std::string json::value::to_string() const
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
		return _object_ptr->to_string();
	case value_type::Array:
		return _array_ptr->to_string();
	default:
		throw exception("Unknown Value Type");
	}
}

const std::string json::value::format(std::string shift_str, size_t basic_shift_count) const
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
		return _object_ptr->format(shift_str, basic_shift_count);
	case value_type::Array:
		return _array_ptr->format(shift_str, basic_shift_count);
	default:
		throw exception("Unknown Value Type");
	}
}

json::value& json::value::operator=(const value & rhs)
{
	_type = rhs._type;
	_raw_data = rhs._raw_data;
	_array_ptr = copy_unique_ptr(rhs._array_ptr);
	_object_ptr = copy_unique_ptr(rhs._object_ptr);

	return *this;
}

json::value& json::value::operator=(value && rhs) noexcept = default;

const json::value& json::value::operator[](size_t pos) const
{
	if (_type == value_type::Array && _array_ptr != nullptr)
	{
		return _array_ptr->operator[](pos);
	}
	// Array not support to create by operator[]

	throw exception("Wrong Type");
}

json::value& json::value::operator[](size_t pos)
{
	if (_type == value_type::Array && _array_ptr != nullptr)
	{
		return _array_ptr->operator[](pos);
	}
	// Array not support to create by operator[]

	throw exception("Wrong Type");
}

json::value& json::value::operator[](const std::string& key)
{
	static std::mutex create_mutex;
	std::unique_lock<std::mutex> create_lock(create_mutex);

	if (_type == value_type::Object && _object_ptr != nullptr)
	{
		create_lock.unlock();
		return _object_ptr->operator[](key);
	}
	// Create a new value by operator[]
	else if (_type == value_type::Null)
	{
		_type = value_type::Object;
		_object_ptr = std::make_unique<object>();
		create_lock.unlock();
		return _object_ptr->operator[](key);
	}
	create_lock.unlock();

	throw exception("Wrong Type");
}

json::value& json::value::operator[](std::string&& key)
{
	static std::mutex create_mutex;
	std::unique_lock<std::mutex> create_lock(create_mutex);

	if (_type == value_type::Object && _object_ptr != nullptr)
	{
		create_lock.unlock();
		return _object_ptr->operator[](std::move(key));
	}
	// Create a new value by operator[]
	else if (_type == value_type::Null)
	{
		_type = value_type::Object;
		_object_ptr = std::make_unique<object>();
		create_lock.unlock();
		return _object_ptr->operator[](std::move(key));
	}
	create_lock.unlock();

	throw exception("Wrong Type");
}

const json::value json::invalid_value()
{
	return value(value_type::Invalid, std::string());
}

std::ostream& operator<<(std::ostream & out, const json::value & val)
{
	// TODO: format output

	out << val.to_string();
	return out;
}

// std::istream &operator>>(std::istream &in, json::value &val)
// {
//     return in;
// }