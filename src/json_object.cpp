#include "json_object.h"
#include "json_value.h"

#include <utility>
#include <regex>

// To do
bool json::object::parse(const std::string &content)
{
    const std::string reg_json_whitespace = "(?:\\s*)";

    const std::string reg_json_string = "(?:\"(.*)\")";

    const std::string reg_json_object_pair = "(?:" + reg_json_string + reg_json_whitespace + "\\: " + reg_json_whitespace + "(.+?))";
    const std::string reg_json_object = "\\{" + reg_json_whitespace + "(?:(?:" + reg_json_object_pair + reg_json_whitespace + "," + reg_json_whitespace + ")*" + reg_json_object_pair + ")?" + reg_json_whitespace + "\\}";

    std::smatch match_result;
    if (std::regex_match(content, match_result, std::regex(reg_json_object)))
    {
        for (auto it = match_result.begin() + 1; it != match_result.end() - 1 && it != match_result.end(); ++it)
        {
            std::string key = *it;
            ++it; // move to value
            json::value value;
            bool sub_parse = value.parse(*it);
            if (sub_parse == false)
            {
                m_valid = false;
                m_map.clear();
                break;
            }
            m_map[key] = value;
        }
        m_valid = true;
    }
    else
    {
        m_valid = false;
    }

    return m_valid;
}

bool json::object::valid() const
{
    return m_valid;
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

json::value &json::object::operator[](const std::string &key)
{
    return m_map[key];
}