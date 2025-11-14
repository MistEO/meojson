#include "json.hpp"

#include <iostream>

bool include_test()
{
    json::value j;
    j["Hi"] = "meojson";
    std::cout << j << std::endl;

    return true;
}
