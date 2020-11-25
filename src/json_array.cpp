#include "json_array.h"

#include "json_value.h"

json::array::array(const raw_array &arr)
    : _array_data(arr)
{
    ;
}

json::array::array(raw_array &&arr) noexcept
    : _array_data(std::forward<raw_array>(arr))
{
    ;
}

json::array::array(std::initializer_list<raw_array::value_type> init_list)
    : _array_data(init_list)
{
}

// bool json::array::empty() const noexcept
// {
//     return _array_data.empty();
// }

// size_t json::array::size() const noexcept
// {
//     return _array_data.size();
// }

const json::value &json::array::at(size_t pos) const
{
    return _array_data.at(pos);
}

void json::array::clear() noexcept
{
    _array_data.clear();
}

void json::array::push_back(const json::value &val)
{
    _array_data.emplace_back(val);
}

void json::array::push_back(json::value &&val)
{
    _array_data.emplace_back(
        std::forward<json::value>(val));
}

void json::array::emplace_back(const json::value &val)
{
    _array_data.emplace_back(val);
}

void json::array::emplace_back(json::value &&val)
{
    _array_data.emplace_back(
        std::forward<json::value>(val));
}

std::string json::array::to_string() const
{
    std::string str = "[";
    for (auto &&val : _array_data)
    {
        str += val.to_string() + ",";
    }
    if (str.back() == ',')
    {
        str.pop_back();
    }
    str += "]";
    return str;
}

std::string json::array::format(std::string shift_str, size_t basic_shift_count) const
{
    std::string shift;
    for (size_t i = 0; i != basic_shift_count + 1; ++i)
    {
        shift += shift_str;
    }

    std::string str = "[";
    for (auto &&val : _array_data)
    {
        str += "\n" + shift + val.format(shift_str, basic_shift_count + 1) + ",";
    }
    if (str.back() == ',')
    {
        str.pop_back(); // pop last ','
    }

    str += '\n';
    for (size_t i = 0; i != basic_shift_count; ++i)
    {
        str += shift_str;
    }
    str += ']';
    return str;
}

json::array::iterator json::array::begin() noexcept
{
    return _array_data.begin();
}

json::array::iterator json::array::end() noexcept
{
    return _array_data.end();
}

json::array::const_iterator json::array::cbegin() const noexcept
{
    return _array_data.cbegin();
}

json::array::const_iterator json::array::cend() const noexcept
{
    return _array_data.cend();
}

json::array::reverse_iterator json::array::rbegin() noexcept
{
    return _array_data.rbegin();
}
json::array::reverse_iterator json::array::rend() noexcept
{
    return _array_data.rend();
}

json::array::const_reverse_iterator json::array::crbegin() const noexcept
{
    return _array_data.crbegin();
}

json::array::const_reverse_iterator json::array::crend() const noexcept
{
    return _array_data.crend();
}

json::value &json::array::operator[](size_t pos)
{
    return _array_data[pos];
}

const json::value &json::array::operator[](size_t pos) const
{
    return _array_data[pos];
}

// const raw_array &json::array::raw_data() const
// {
//     return _array_data;
// }

std::ostream &operator<<(std::ostream &out, const json::array &arr)
{
    // TODO: format output

    out << arr.to_string();
    return out;
}