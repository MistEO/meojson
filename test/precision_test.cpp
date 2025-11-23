#include "json.hpp"

#include <iomanip>
#include <iostream>
#include <limits>

bool precision_test()
{
    std::cout << "Testing floating point precision..." << std::endl;
    double value = 3.141592653589793;
    json::object obj_old = json::object { { "double", value } };
    std::string obj_str = obj_old.to_string();
    std::cout << "Serialized: " << obj_str << std::endl;
    
    json::object obj_new = json::parse(obj_str).value().as_object();
    double new_value = obj_new.at("double").as_double();
    
    std::cout << "Original: " << std::hexfloat << value << std::defaultfloat << std::endl;
    std::cout << "Restored: " << std::hexfloat << new_value << std::defaultfloat << std::endl;
    
    if (new_value != value) {
        std::cerr << "Precision loss detected!" << std::endl;
        std::cerr << "Diff: " << (new_value - value) << std::endl;
        return false;
    }
    
    std::cout << "Precision test passed" << std::endl;
    return true;
}
