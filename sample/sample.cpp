#include <iostream>
#include <fstream>
#include <sstream>

#include "json.h"

int main()
{

    /*** Parse ***/
    {
        std::ifstream ifs("test.json");
        std::stringstream ibuf;
        ibuf << ifs.rdbuf();
        std::string content(ibuf.str());
        //std::cout << content << std::endl;

        json::value json = json::parser::parse(content);

        std::cout << "parse success" << std::endl;
        std::cout << json.as_object()["configurations"].as_array().at(0).as_object()["name"].as_string() << std::endl;
#if __cplusplus >= 201703L // C++17
        for (auto &&[key, val] : json.as_object()["configurations"].as_array().at(0).as_object())
        {
            std::cout << key << ": " << val.to_string() << std::endl;
        }
#elif __cplusplus >= 201103L // C++11
        for (auto &&pair : json.as_object()["configurations"].as_array().at(0).as_object())
        {
            std::cout << pair.first << ": " << pair.second.to_string() << std::endl;
        }
#endif
    }

    /*** Generate ***/
    {
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
        // json::array arr = {json::value::string("123"), json::value::number(456)};
        // obj["arr"] = arr;

        std::cout << obj.to_string() << std::endl;
    }

    return 0;
}
