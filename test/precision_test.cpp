#define MEOJSON_KEEP_FLOATING_PRECISION

#include "../include/json.hpp"

#include <iomanip>
#include <iostream>

bool precision_test()
{
    double value = 3.141592653589793;
    json::object obj_old = json::object { { "double", value } };
    std::string obj_str = obj_old.to_string();
    std::cout << obj_str << std::endl;
    json::object obj_new = json::parse(obj_str).value().as_object();
    std::cout << "old:" << std::hexfloat << value << std::endl;
    std::cout << "new:" << std::hexfloat << obj_new.at("double").as_double() << std::endl;
    return obj_new.at("double").as_double() == value;
}
