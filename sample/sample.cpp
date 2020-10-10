#include <iostream>
#include <fstream>
#include <sstream>

#include "json.h"

int main()
{
    /*** Parse: String to Json ***/
    std::ifstream ifs("test.json");
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    std::cout << content << std::endl;

    json::value val;
    val = json::parser::parse(content);
    std::cout << "parse success" << std::endl;
    std::cout << val.as_object()["configurations"].as_array()[0].as_object()["name"].as_string() << std::endl;

    /*** Ify: Json to String  ***/
    json::object obj;
    obj["key1"] = json::value::string("hello");
    obj["PI"] = json::value::number(3.1416);
    std::vector<json::value> value_vector;
    for (int i = 0; i != 10; ++i)
    {
        value_vector.push_back(json::value::number(i));
    }
    obj["list"] = json::array(value_vector);

    json::object obj2;
    obj2["obj2_key1"] = json::value::string("i am child");
    obj["child"] = obj2;

    std::cout << obj.to_string() << std::endl;

    return 0;
}