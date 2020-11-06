#include <iostream>
#include <string>

#include "json.h"

std::string generate_string(int max_length)
{
    std::string res;
    int times = rand() % max_length;
    for (int i = 0; i != times; ++i)
    {
        switch ((rand() % 3))
        {
        case 1:
            res += 'A' + rand() % 26;
            break;
        case 2:
            res += 'a' + rand() % 26;
            break;
        default:
            res += '0' + rand() % 10;
            break;
        }
    }
    return res;
}

json::value generate_value(int depth);

json::object generate_object(int depth)
{
    constexpr int max_length = 64;
    json::object obj;
    if (depth > 2)
    {
        return obj;
    }
    int times = rand() % max_length;
    if (depth == 0)
    {
        times = 64;
    }
    for (int i = 0; i != times; ++i)
    {
        std::string key = generate_string(64);
        json::value value = generate_value(depth);
        obj.insert(std::move(key), std::move(value));
    }
    return obj;
}

json::array generate_array(int depth)
{
    constexpr int max_length = 128;
    json::array arr;
    if (depth > 2)
    {
        return arr;
    }
    int times = rand() % max_length;
    for (int i = 0; i != times; ++i)
    {
        json::value value = generate_value(depth);
        arr.push_back(std::move(value));
    }
    return arr;
}

json::value generate_value(int depth)
{
    int type = rand() % 6;
    switch (static_cast<json::ValueType>(type + 1))
    {
    case json::ValueType::JsonWhiteSpace:
        return json::value::null();
    case json::ValueType::JsonNull:
        return json::value::null();
    case json::ValueType::JsonBoolean:
        return json::value::boolean(rand() % 1);
    case json::ValueType::JsonString:
        return json::value::string(generate_string(4096));
    case json::ValueType::JsonNumber:
        return json::value::number(rand());
    case json::ValueType::JsonArray:
        return generate_array(depth + 1);
    case json::ValueType::JsonObject:
        return generate_object(depth + 1);
    default:
        return json::value::null();
    }
}

int main()
{
    srand(1024);
    json::object root = generate_object(0);
    std::cout << root.to_string() << std::endl;

    return 0;
}