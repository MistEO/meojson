#include "json_object.h"

#include <regex>
#include <utility>
#include <algorithm>

#include "json_value.h"
#include "json_exception.h"

#ifdef DEBUG
#include <iostream>
#endif

const json::basic_json json::object::at(const std::string &key) const
{
    return m_map.at(key);
}

bool json::object::insert(const std::string &key, const json::value &value)
{
    return m_map.insert(std::make_pair(key, value)).second;
}

bool json::object::earse(const std::string &key)
{
    return m_map.erase(key) > 0 ? true : false;
}

std::string json::object::to_string() const
{
    std::string str = "{ ";
    for (auto iter = m_map.cbegin(); iter != m_map.cend(); ++iter)
    {
        if (iter != m_map.cbegin())
        {
            str += ", ";
        }
        str += "\"" + iter->first + "\": " + iter->second.to_string();
    }
    str += " }";
    return str;
}

json::basic_json &json::object::operator[](const std::string &key)
{
    return m_map[key];
}

const json::basic_json &json::object::operator[](const std::string &key) const
{
    return m_map.at(key);
}
