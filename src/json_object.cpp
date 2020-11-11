#include "json_object.h"

#include "json_value.h"

json::object::object(const json_object &map)
    : _object_data(map)
{
    ;
}

json::object::object(json_object &&map)
    : _object_data(map)
{
    ;
}

bool json::object::empty() const
{
    return _object_data.empty();
}

const json::value json::object::at(const std::string &key) const
{
    return _object_data.at(key);
}

size_t json::object::size() const
{
    return _object_data.size();
}

void json::object::clear()
{
    _object_data.clear();
}

bool json::object::insert(const std::string &key, const json::value &value)
{
    return _object_data.insert(std::make_pair(key, value)).second;
}

bool json::object::insert(std::string &&key, json::value &&value)
{
    return _object_data.insert(
                           std::make_pair(
                               std::forward<std::string>(key),
                               std::forward<json::value>(value)))
        .second;
}

bool json::object::earse(const std::string &key)
{
    return _object_data.erase(key) > 0 ? true : false;
}

std::string json::object::to_string() const
{
    std::string str = "{";
    for (auto iter = _object_data.cbegin(); iter != _object_data.cend(); ++iter)
    {
        if (iter != _object_data.cbegin())
        {
            str += ",";
        }
        str += "\"" + iter->first + "\":" + iter->second.to_string();
    }
    str += "}";
    return str;
}

json::object::iterator json::object::begin()
{
    return _object_data.begin();
}

json::object::iterator json::object::end()
{
    return _object_data.end();
}

json::object::const_iterator json::object::cbegin() const
{
    return _object_data.cbegin();
}

json::object::const_iterator json::object::cend() const
{
    return _object_data.cend();
}

json::value &json::object::operator[](const std::string &key)
{
    return _object_data[key];
}

const json::value &json::object::operator[](const std::string &key) const
{
    return _object_data.at(key);
}

// const json_object &json::object::raw_data() const
// {
//     return _object_data;
// }

std::ostream &operator<<(std::ostream &out, const json::object &object)
{
    // TODO: format output

    out << object.to_string();
    return out;
}