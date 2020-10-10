#pragma once

#include <map>

#include "basic_json.h"

namespace json
{
    class value;

    class object : public basic_json
    {
    public:
        object() = default;
        object(const object &rhs) = default;

        ~object() = default;

        std::string to_string() const override;

        const json::basic_json at(const std::string &key) const;
        bool insert(const std::string &key, const json::value &value);
        bool earse(const std::string &key);

        json::basic_json &operator[](const std::string &key);
        const json::basic_json &operator[](const std::string &key) const;

    private:
        std::map<std::string, json::basic_json> m_map;
        bool m_valid = true;
    };
} // namespace json