#include <iostream>
#include <fstream>
#include <sstream>

#include "json.h"

int main()
{
    json::object root;
    root["str"] = json::value::string("plain text");
    root["num"] = json::value::number(3.14);
    // std::cout << root.to_string() << std::endl;

    std::ifstream ifs("test.json");
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    std::cout << content << std::endl;

    json::value json;
    bool parse_ret = json.parse(content);
    std::cout << "parse ret : " << (parse_ret ? "true" : "false") << std::endl;
    std::cout << json.as_object()["configurations"].as_array().at(0).as_object()["name"].as_string() << std::endl;

    return 0;
}