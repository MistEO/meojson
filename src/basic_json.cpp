#include "basic_json.h"

bool json::basic_json::valid() const
{
    return m_type != json::JsonInvalid;
}

json::ValueType json::basic_json::type() const
{
    return m_type;
}

std::string json::basic_json::to_string() const
{
    return m_raw;
}