#pragma once

#include <string>
#include <map>

namespace json
{
    class value;

    class object
    {
    public:
        object() = default;
        object(const object &rhs) = default;
        object(const std::map<std::string, value> &value_map);

        ~object() = default;

        bool empty() const;

        const value at(const std::string &key) const;
        bool insert(const std::string &key, const value &value);
        bool earse(const std::string &key);

        std::string to_string() const;

        value &operator[](const std::string &key);
        const value &operator[](const std::string &key) const;

        const std::map<std::string, value> &raw_data() const;

    private:
        std::map<std::string, value> _object_data;
        bool _valid = false;
    };
} // namespace json