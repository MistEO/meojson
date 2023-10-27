#include "json.hpp"

#include <iostream>

bool include_test()
{
    json::value j;
    j["hi"] = "meojson";
    std::cout << j << std::endl;

    return true;
}
