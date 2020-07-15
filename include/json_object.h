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

        ~object() = default;

        bool parse(const std::string &content);
        bool valid() const;

        json::value at(const std::string &key) const;
        bool insert(const std::string &key, const json::value &value);
        bool earse(const std::string &key);

        std::string to_string() const;

        json::value &operator[](const std::string &key);

    private:
        std::map<std::string, json::value> m_map;
        bool m_valid = true;
    };
} // namespace json