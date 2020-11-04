#pragma once

#include <string>
#include <vector>

namespace json
{
    class value;

    class array
    {
        friend class value;

    public:
        array() = default;
        array(const array &rhs) = default;
        array(array &&rhs) = default;
        array(const std::vector<value> &value_array);
        array(std::vector<value> &&value_array);

        ~array() = default;

        bool empty() const;
        const value at(int index) const;
        void push_back(const value &value);
        void push_back(value &&value);
        // void earse(int index);

        std::string to_string() const;

        value &operator[](int index);
        const value &operator[](int index) const;
        array &operator=(const array &) = default;
        array &operator=(array &&) = default;
        // const std::vector<value> &raw_data() const;

    private:
        std::vector<value> _array_data;
    };
} // namespace json