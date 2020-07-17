#include "json_exception.h"

json::exception::exception(const std::string &msg)
    : m_msg(msg)
{
}

json::exception::~exception() noexcept
{
}

const char *json::exception::what() const noexcept
{
    return m_msg.c_str();
}