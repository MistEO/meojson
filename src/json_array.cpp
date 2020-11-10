#include "json_array.h"

#include "json_value.h"

json::array::array(const std::vector<json::value> &value_array)
    : _array_data(value_array)
{
    ;
}

json::array::array(std::vector<json::value> &&value_array)
    : _array_data(value_array)
{
    ;
}

json::array::array(std::initializer_list<json::value> value_list)
    : _array_data(value_list)
{
}

bool json::array::empty() const
{
    return _array_data.empty();
}

const json::value json::array::at(int index) const
{
    return _array_data.at(index);
}

size_t json::array::size() const
{
    return _array_data.size();
}

void json::array::push_back(const json::value &value)
{
    _array_data.push_back(value);
}

void json::array::push_back(json::value &&value)
{
    _array_data.push_back(
        std::forward<json::value>(value));
}

std::string json::array::to_string() const
{
    std::string str = "[";
    for (auto iter = _array_data.cbegin(); iter != _array_data.cend(); ++iter)
    {
        if (iter != _array_data.cbegin())
        {
            str += ",";
        }
        str += iter->to_string();
    }
    str += "]";
    return str;
}

json::array::iterator json::array::begin()
{
    return _array_data.begin();
}

json::array::iterator json::array::end()
{
    return _array_data.end();
}

json::array::const_iterator json::array::cbegin() const
{
    return _array_data.cbegin();
}

json::array::const_iterator json::array::cend() const
{
    return _array_data.cend();
}

json::array::reverse_iterator json::array::rbegin()
{
    return _array_data.rbegin();
}
json::array::reverse_iterator json::array::rend()
{
    return _array_data.rend();
}

json::array::const_reverse_iterator json::array::crbegin() const
{
    return _array_data.crbegin();
}

json::array::const_reverse_iterator json::array::crend() const
{
    return _array_data.crend();
}

json::value &json::array::operator[](int index)
{
    return _array_data[index];
}

const json::value &json::array::operator[](int index) const
{
    return _array_data.at(index);
}

// const std::vector<json::value> &json::array::raw_data() const
// {
//     return _array_data;
// }

std::ostream &operator<<(std::ostream &out, const json::array &array)
{
    // TODO: format output

    out << array.to_string();
    return out;
}