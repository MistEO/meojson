#include <iostream>
#include <fstream>
#include <sstream>

#include "json.h"

int main()
{
    std::ifstream ifs("test.json");
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    std::cout << content << std::endl;

    json::value val;
    val = json::parser::parse(content);
    std::cout << "parse success" << std::endl;
    std::cout << val.as_object()["configurations"].as_array()[0].as_object()["name"].as_string() << std::endl;

    return 0;
}