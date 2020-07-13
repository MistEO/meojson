#include "json_array.h"

json::array::array(const std::vector<json::value> &value_array)
    : m_vector(value_array)
{
    ;
}