#pragma once

#include <string>
#include <vector>
#include "json_value.h"

namespace json
{
    class array
    {
    public:
        array() = default;
        array(const array &rhs) = default;
        array(const std::vector<json::value> &value_array);

        ~array() = default;

        json::value at(int index) const;

        json::value &operator[](int index);

    private:
        std::vector<json::value> m_vector;
    };
} // namespace json