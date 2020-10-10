#include "json_object.h"

#include "json_value.h"

json::object::object(const std::map<std::string, json::value> &value_map)
    : _object_data(value_map)
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

bool json::object::insert(const std::string &key, const json::value &value)
{
    return _object_data.insert(std::make_pair(key, value)).second;
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

json::value &json::object::operator[](const std::string &key)
{
    return _object_data[key];
}

const json::value &json::object::operator[](const std::string &key) const
{
    return _object_data.at(key);
}

const std::map<std::string, json::value> &json::object::raw_data() const
{
    return _object_data;
}