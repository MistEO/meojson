#include <iostream>

#include "json.hpp"
#include "parse_test.h"

bool wrong_json();
bool jsonc_trail_comma();
bool jsonc_comment();

bool parsing()
{
    return wrong_json() && jsonc_trail_comma() && jsonc_comment();
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

bool jsonc_trail_comma()
{
    std::string obj_trail = R"({ "key": 1, })";
    if (!json::parsec(obj_trail)) {
        std::cerr << "Parsing failed:" << obj_trail << std::endl;
        return false;
    }

    std::string arr_trail = R"([ 1, 2, ])";
    if (!json::parsec(arr_trail)) {
        std::cerr << "Parsing failed:" << arr_trail << std::endl;
        return false;
    }

    return true;
}

bool jsonc_comment()
{
    std::string json = R"(
/*123//321
123*/{ "key"://456/*789
 1, }
/* trail */
)";
    if (!json::parsec(json)) {
        std::cerr << "Parsing failed:" << json << std::endl;
        return false;
    }

    return true;
}
