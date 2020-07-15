#include "json_array.h"

json::array::array(const std::vector<json::value> &value_array)
    : m_vector(value_array)
{
    ;
}
// To do
bool json::array::parse(const std::string &content)
{

    return true;
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