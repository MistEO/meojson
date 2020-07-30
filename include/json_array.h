#pragma once

#include <string>
#include <vector>

namespace json
{
    class value;
    
    class array
    {
    public:
        array() = default;
        array(const array &rhs) = default;
        array(const std::vector<json::value> &value_array);

        ~array() = default;

        bool parse(const std::string &content);
        bool valid() const;

        json::value at(int index) const;

        std::string to_string() const;

        json::value &operator[](int index);

    private:
        std::vector<json::value> m_vector;
        bool m_valid = true;
    };
} // namespace json