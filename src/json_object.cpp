#include "json_object.h"

#include <regex>
#include <utility>
#include <algorithm>

#include "json_value.h"

#ifdef DEBUG
#include <iostream>
#endif

bool json::object::parse(const std::string &content)
{
    std::string format_content(content);
    std::replace(format_content.begin(), format_content.end(), '\n', ' ');

    // Reference
    // Json     https://www.json.org/json-zh.html
    // Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
    static const std::string reg_str_json_whitespace = "(?:\\s*)";

    static const std::string reg_str_json_null = "(?:null)";
    static const std::string reg_str_json_boolean = "(?:true|false)";
    static const std::string reg_str_json_string = "(?:\".*?\")";
    static const std::string reg_str_json_string_capturing = "(?:\"(.*?)\")";

    static const std::string reg_str_json_number_fraction = "(?:\\.\\d+)?";
    static const std::string reg_str_json_number_exponent = "(?:(?:e|E)(?:-|\\+)?\\d+)?";
    static const std::string reg_str_json_number = "(?:-?\\d+" + reg_str_json_number_fraction + reg_str_json_number_exponent + ")";

    static const std::string reg_str_json_value = "(" + reg_str_json_null + "|" + reg_str_json_string + "|" + reg_str_json_number + "|(?:\\{.*?\\})|(?:\\[.*?\\]))";

    static const std::string reg_str_json_object_pair = "(?:" + reg_str_json_string_capturing + reg_str_json_whitespace + "\\: " + reg_str_json_whitespace + reg_str_json_value + ")";
    static const std::string reg_str_json_object = "\\{" + reg_str_json_whitespace + "(?:(?:" + reg_str_json_object_pair + reg_str_json_whitespace + "," + reg_str_json_whitespace + ")*" + reg_str_json_object_pair + ")?" + reg_str_json_whitespace + "\\}";

    static const std::regex reg_json_object("^" + reg_str_json_object + "$");

#ifdef DEBUG
    std::cout << reg_str_json_object << std::endl;
#endif

    std::smatch match_result;
    if (std::regex_match(format_content, match_result, reg_json_object))
    {
#ifdef DEBUG
        std::cout << "========START DEBUG========" << std::endl;
        for (auto dbit = match_result.begin(); dbit != match_result.end(); ++dbit)
        {
            std::cout << "===" << *dbit << std::endl;
        }
        std::cout << "========END DEBUG========" << std::endl;
#endif
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