#include "json.hpp"

#include <iomanip>
#include <iostream>

bool unicode_test()
{
    std::string test = R"({ "test": "abc\u9a8fdef" })";
    auto obj = json::parse(test);
    if (!obj.has_value()) {
        std::cout << "parse failed" << std::endl;
        return false;
    }
    for (auto ch : obj.value().at("test").as_string()) {
        std::cout << std::hex << std::setw(2)
                  << static_cast<unsigned>(static_cast<unsigned char>(ch)) << ' ';
    }
    std::cout << std::endl;
    for (auto ch : "abc\u9a8fdef") {
        std::cout << std::hex << std::setw(2)
                  << static_cast<unsigned>(static_cast<unsigned char>(ch)) << ' ';
    }
    std::cout << std::endl;
    return obj.value().at("test").as_string() == "abc\u9a8fdef";
}
