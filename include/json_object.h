#pragma once

#include <string>
#include <map>

namespace json
{
    class value;

    class object
    {
        friend class value;

    public:
        typedef std::map<std::string, value>::iterator iterator;
        typedef std::map<std::string, value>::const_iterator const_iterator;
        typedef std::map<std::string, value>::reverse_iterator reverse_iterator;
        typedef std::map<std::string, value>::const_reverse_iterator const_reverse_iterator;

        object() = default;
        object(const object &rhs) = default;
        object(object &&rhs) = default;
        object(const std::map<std::string, value> &value_map);
        object(std::map<std::string, value> &&value_map);

        ~object() = default;

        bool empty() const;
        const value at(const std::string &key) const;
        size_t size() const;
        std::string to_string() const;

        void clear();
        bool insert(const std::string &key, const value &value);
        bool insert(std::string &&key, value &&value);
        bool earse(const std::string &key);

        iterator begin();
        iterator end();
        const_iterator cbegin() const;
        const_iterator cend() const;

        reverse_iterator rbegin();
        reverse_iterator rend();
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        value &operator[](const std::string &key);
        const value &operator[](const std::string &key) const;
        object &operator=(const object &) = default;
        object &operator=(object &&) = default;

        // const std::map<std::string, value> &raw_data() const;

    private:
        std::map<std::string, value> _object_data;
        bool _valid = false;
    };
} // namespace json