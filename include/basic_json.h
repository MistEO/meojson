#pragma once

#include <string>

namespace json
{
    enum ValueType
    {
        JsonInvalid,
        JsonNull,
        JsonBoolean,
        JsonString,
        JsonNumber,
        JsonArray,
        JsonObject
    };

    class basic_json
    {
    public:
        basic_json() = default;
        basic_json(const basic_json &rhs) = default;

        virtual ~basic_json() = default;

        virtual bool valid() const;
        ValueType type() const;

        virtual std::string to_string() const;

    protected:
        std::string m_raw;
        ValueType m_type;
    };
} // namespace json