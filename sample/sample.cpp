#include <iostream>
#include <fstream>

#include "json.h"

int main()
{
    json::object root;
    root["str"] = json::value::string("plain text");
    root["num"] = json::value::number(3.14);
    std::cout << root.to_string() << std::endl;

    std::ifstream in("test.json", std::ios_base::in);

    std::string content = "{ \" aaa \" : 123 , \" bbb \" : 456 }";
    json::value json;
    json.parse(content);

    return 0;
}