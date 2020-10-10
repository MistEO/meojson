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
        object(const std::map<std::string, json::value> &value_map);

        ~object() = default;

        bool empty() const;

        const json::value at(const std::string &key) const;
        bool insert(const std::string &key, const json::value &value);
        bool earse(const std::string &key);

        std::string to_string() const;

        json::value &operator[](const std::string &key);
        const json::value &operator[](const std::string &key) const;

        const std::map<std::string, json::value> &raw_data() const;

    private:
        std::map<std::string, json::value> _object_data;
        bool _valid = false;
    };
} // namespace json