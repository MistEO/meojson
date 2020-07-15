#include <iostream>
#include <fstream>

#include "json.h"

int main()
{
    json::object root;
    root["str"] = json::value::string("plain text");
    root["num"] = json::value::number(3.14);
    std::cout << root.to_string() << std::endl;

    std::string content = "   ";
    json::value json;
    json.parse(content);

    return 0;
}