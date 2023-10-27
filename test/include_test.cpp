#include "json.hpp"
#include "json5.hpp"

#include <iostream>

bool include_test()
{
    json::value j;
    j["Hi"] = "meojson";
    std::cout << j << std::endl;

    auto opt5 = json::parse5(R"({"Hello":"World"})");
    std::cout << *opt5 << std::endl;

    return true;
}
