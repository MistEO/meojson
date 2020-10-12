#pragma once

namespace json
{
    enum ValueType
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