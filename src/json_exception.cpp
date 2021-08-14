#include "json_exception.h"

json::exception::exception(const std::string& msg)
    : m_msg(msg)
{
    ;
}

const char* json::exception::what() const noexcept
{
    return m_msg.c_str();
}