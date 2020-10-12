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
        array(const std::vector<value> &value_array);

        ~array() = default;

        bool empty() const;
        const value at(int index) const;
        void push_back(const value &value);
        // void earse(int index);

        std::string to_string() const;

        value &operator[](int index);
        const value &operator[](int index) const;

        const std::vector<value> &raw_data() const;

    private:
        std::vector<value> _array_data;
    };
} // namespace json