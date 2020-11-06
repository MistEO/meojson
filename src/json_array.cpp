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