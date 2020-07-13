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

        json::value at(const std::string &key) const;
        bool insert(const std::string &key, const json::value &value);
        bool earse(const std::string &key);

        json::value &operator[](const std::string &key);

    private:
        std::map<std::string, json::value> m_map;
    };
} // namespace json