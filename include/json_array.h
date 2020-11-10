#pragma once

#include <string>
#include <vector>
#include <initializer_list>

namespace json
{
    class value;

    class array
    {
        friend class value;

    public:
        typedef std::vector<value>::iterator iterator;
        typedef std::vector<value>::const_iterator const_iterator;
        typedef std::vector<value>::reverse_iterator reverse_iterator;
        typedef std::vector<value>::const_reverse_iterator const_reverse_iterator;

        array() = default;
        array(const array &rhs) = default;
        array(array &&rhs) = default;
        array(const std::vector<value> &value_array);
        array(std::vector<value> &&value_array);
        array(std::initializer_list<value> value_list);

        ~array() = default;

        bool empty() const;
        const value at(int index) const;
        size_t size() const;
        std::string to_string() const;

        void push_back(const value &value);
        void push_back(value &&value);
        // void earse(int index);

        iterator begin();
        iterator end();
        const_iterator cbegin() const;
        const_iterator cend() const;

        reverse_iterator rbegin();
        reverse_iterator rend();
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        value &operator[](int index);
        const value &operator[](int index) const;
        array &operator=(const array &) = default;
        array &operator=(array &&) = default;

        // const std::vector<value> &raw_data() const;

    private:
        std::vector<value> _array_data;
    };

    std::ostream &operator<<(std::ostream &out, const array &array);

} // namespace json