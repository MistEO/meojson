#include "json_object.h"

#include "json_value.h"

json::object::object(const raw_object &raw_obj)
    : _object_data(raw_obj)
{
    ;
}

json::object::object(raw_object &&raw_obj)
    : _object_data(std::forward<raw_object>(raw_obj))
{
    ;
}

json::object::object(std::initializer_list<raw_object::value_type> init_list)
{
    for (auto &&[key, val] : init_list)
    {
        insert(key, val);
    }
}

bool json::object::empty() const noexcept
{
    return _object_data.empty();
}

const json::value &json::object::at(const std::string &key) const
{
    return _object_data.at(key);
}

size_t json::object::size() const noexcept
{
    return _object_data.size();
}

void json::object::clear() noexcept
{
    _object_data.clear();
}

bool json::object::insert(const std::string &key, const json::value &value)
{
    return _object_data.emplace(key, value).second;
}

bool json::object::insert(std::string &&key, json::value &&val)
{
    return _object_data.emplace(
                           std::forward<std::string>(key),
                           std::forward<json::value>(val))
        .second;
}

bool json::object::emplace(const std::string &key, const json::value &value)
{
    return _object_data.emplace(key, value).second;
}

bool json::object::emplace(std::string &&key, json::value &&val)
{
    return _object_data.emplace(
                           std::forward<std::string>(key),
                           std::forward<json::value>(val))
        .second;
}

bool json::object::earse(const std::string &key)
{
    return _object_data.erase(key) > 0 ? true : false;
}

std::string json::object::to_string() const
{
    std::string str = "{";
    for (auto &&[key, val] : _object_data)
    {
        str += "\"" + key + "\":" + val.to_string() + ",";
    }
    str.pop_back();
    str += "}";
    return str;
}

std::string json::object::format(std::string shift_str, size_t basic_shift_count) const
{
    std::string shift;
    for (size_t i = 0; i != basic_shift_count + 1; ++i)
    {
        shift += shift_str;
    }

    std::string str = "{";
    for (auto &&[key, val] : _object_data)
    {
        str += "\n" + shift + "\"" + key + "\": " + val.format(shift_str, basic_shift_count + 1) + ",";
    }
    str.pop_back(); // pop last ','

    str += '\n';
    for (size_t i = 0; i != basic_shift_count; ++i)
    {
        str += shift_str;
    }
    str += '}';
    return str;
}

json::object::iterator json::object::begin() noexcept
{
    return _object_data.begin();
}

json::object::iterator json::object::end() noexcept
{
    return _object_data.end();
}

json::object::const_iterator json::object::cbegin() const noexcept
{
    return _object_data.cbegin();
}

json::object::const_iterator json::object::cend() const noexcept
{
    return _object_data.cend();
}

json::value &json::object::operator[](const std::string &key)
{
    return _object_data[key];
}

json::value &json::object::operator[](std::string &&key)
{
    return _object_data[std::forward<std::string>(key)];
}

// const raw_object &json::object::raw_data() const
// {
//     return _object_data;
// }

std::ostream &operator<<(std::ostream &out, const json::object &obj)
{
    // TODO: format output

    out << obj.to_string();
    return out;
}