#include <iostream>
#include <string>
#include <ctime>

#include "json.h"

std::string rand_string(int max_length);
json::value rand_value(int depth);
json::object rand_object(int depth);
json::array rand_array(int depth);

constexpr int max_key_length = 64;
constexpr int max_string_length = 4096;

int max_depth = 2;
int max_element_count = 256;

int main(int argc, char* argv[])
{
    srand(time(NULL));
    if (argc == 3) {
        max_depth = atoi(argv[1]);
        max_element_count = atoi(argv[2]);
    }
    json::object root = rand_object(0);
    std::cout << root.to_string() << std::endl;

    return 0;
}

json::value rand_value(int depth)
{
    int type = rand() % static_cast<int>(json::value_type::NUM_T);
    switch (static_cast<json::value_type>(type)) {
    case json::value_type::Null:
        return json::value();
    case json::value_type::Boolean:
        return static_cast<bool>(rand() % 1);
    case json::value_type::String:
        return rand_string(max_string_length);
    case json::value_type::Number:
        return rand();
    case json::value_type::Array:
        return rand_array(depth + 1);
    case json::value_type::Object:
        return rand_object(depth + 1);
    default:
        return json::value();
    }
}

json::object rand_object(int depth)
{
    json::object obj;
    if (depth > 2) {
        return obj;
    }
    int times = rand() % max_element_count;
    if (depth == 0) {
        times = max_element_count;
    }
    for (int i = 0; i != times; ++i) {
        std::string key = rand_string(max_key_length);
        json::value value = rand_value(depth);
        obj.emplace(std::move(key), std::move(value));
    }
    return obj;
}

json::array rand_array(int depth)
{
    json::array arr;
    if (depth > 2) {
        return arr;
    }
    int times = rand() % max_element_count;
    for (int i = 0; i != times; ++i) {
        json::value value = rand_value(depth);
        arr.emplace_back(std::move(value));
    }
    return arr;
}

std::string rand_string(int max_length)
{
    std::string str;
    int times = rand() % max_length;
    for (int i = 0; i != times; ++i) {
        switch ((rand() % 3)) {
        case 1:
            str += 'A' + rand() % 26;
            break;
        case 2:
            str += 'a' + rand() % 26;
            break;
        default:
            str += '0' + rand() % 10;
            break;
        }
    }
    return str;
}
