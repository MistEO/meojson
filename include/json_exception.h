#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace json
{
    class exception : public std::exception
    {
    public:
        exception(const std::string &msg);

        virtual ~exception() noexcept;

        virtual const char *what() const noexcept;

    private:
        std::string m_msg;
    };

} // namespace json