#pragma once

namespace json
{
    enum class ValueType
    {
        JsonWhiteSpace,
        JsonNull,
        JsonBoolean,
        JsonString,
        JsonNumber,
        JsonArray,
        JsonObject
    };
}