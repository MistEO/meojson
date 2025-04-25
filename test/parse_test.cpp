#include <iostream>

#include "json.hpp"
#include "parse_test.h"

bool wrong_json();

bool parsing()
{
    return wrong_json();
}

bool wrong_json()
{
    std::string duplicate_key_json = R"({"key": 1, "key": 2})";
    if (json::parse(duplicate_key_json)) {
        std::cerr << "Parsing failed:" << duplicate_key_json << std::endl;
        return false;
    }

    return true;
}
