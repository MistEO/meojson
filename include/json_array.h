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

        bool empty() const;
        const json::value at(int index) const;
        void push_back(const json::value &value);
        // void earse(int index);

        std::string to_string() const;

        json::value &operator[](int index);
        const json::value &operator[](int index) const;

        const std::vector<json::value> &raw_data() const;

    private:
        std::vector<json::value> _array_data;
    };
} // namespace json