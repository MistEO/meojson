#pragma once

namespace json
{
    enum ValueType
    {
        JsonWhiteSpace,
        JsonInvalid,
        JsonNull,
        JsonBoolean,
        JsonString,
        JsonNumber,
        JsonArray,
        JsonObject
    };
}