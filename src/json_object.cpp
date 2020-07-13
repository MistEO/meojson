#include "json_object.h"
#include "json_value.h"

#include <utility>

bool json::object::parse(const std::string &content)
{
    return true;
}

json::value json::object::at(const std::string &key) const
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

json::value &json::object::operator[](const std::string &key)
{
    return m_map[key];
}