#include "json_array.h"

#include <regex>
#include <utility>
#include <algorithm>

#ifdef DEBUG
#include <iostream>
#endif

#include "json_value.h"

json::array::array(const std::vector<json::value> &value_array)
    : m_vector(value_array)
{
    ;
}

bool json::array::parse(const std::string &content)
{
    std::string format_content(content);
    std::replace(format_content.begin(), format_content.end(), '\n', ' ');

    // Reference
    // Json     https://www.json.org/json-zh.html
    // Regex    https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Regular_Expressions
    static const std::string reg_str_json_whitespace = "(?:\\s*)";

    static const std::string reg_str_json_null = "(?:null)";
    static const std::string reg_str_json_boolean = "(?:true|false)";
    static const std::string reg_str_json_string = "(?:\"[^\"]*\")";

    static const std::string reg_str_json_number_fraction = "(?:\\.\\d+)?";
    static const std::string reg_str_json_number_exponent = "(?:(?:e|E)(?:-|\\+)?\\d+)?";
    static const std::string reg_str_json_number = "(?:-?\\d+" + reg_str_json_number_fraction + reg_str_json_number_exponent + ")";

    static const std::string reg_str_json_value = "(" + reg_str_json_null + "|" + reg_str_json_boolean + "|" + reg_str_json_string + "|" + reg_str_json_number + "|(?:\\{.*\\})|(?:\\[.*\\]))";

    static const std::string reg_str_json_array_element = reg_str_json_value;
    static const std::string reg_str_json_array = "\\[" + reg_str_json_whitespace + "(?:(?:" + reg_str_json_array_element + reg_str_json_whitespace + "," + reg_str_json_whitespace + ")*?" + reg_str_json_array_element + ")?" + reg_str_json_whitespace + "\\]";

    static const std::regex reg_json_array_element(reg_str_json_array_element);
    static const std::regex reg_json_array("^" + reg_str_json_array + "$");

    std::smatch match_result;
    if (std::regex_match(format_content, reg_json_array))
    {
        m_valid = true;
        std::string search_content = format_content.substr(1, format_content.size() - 2);
        while (std::regex_search(search_content, match_result, reg_json_array_element))
        {
            if (match_result.size() == 2)
            {
                json::value value;
                bool sub_parse = value.parse(match_result[1]);
                if (sub_parse == false)
                {
                    m_valid = false;
                    m_vector.clear();
                    break;
                }
                m_vector.push_back(value);
            }
            else
            {
                m_vector.clear();
                m_valid = false;
                break;
            }
            search_content = match_result.suffix().str();
        }
    }
    else
    {
        m_valid = false;
    }

    return m_valid;
}

bool json::array::valid() const
{
    return m_valid;
}

json::value json::array::at(int index) const
{
    return m_vector.at(index);
}

std::string json::array::to_string() const
{
    std::string str = "[ ";
    for (auto iter = m_vector.cbegin(); iter != m_vector.cend(); ++iter)
    {
        if (iter != m_vector.cbegin())
        {
            str += ", ";
        }
        str += iter->to_string();
    }
    str += " ]";
    return str;
}

json::value &json::array::operator[](int index)
{
    return m_vector[index];
}