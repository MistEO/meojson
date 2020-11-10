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

        auto &&[ret, json] = json::parser::parse(content);

        if (ret)
        {
            std::cout << "parse success" << std::endl;
            std::cout << json.as_object()["configurations"].as_array().at(0).as_object()["name"].as_string() << std::endl;
            for (auto &&[key, val] : json.as_object()["configurations"].as_array().at(0).as_object())
            {
                std::cout << key << ": " << val.to_string() << std::endl;
            }
        }
        else
        {
            std::cout << "parse failed" << std::endl;
        }
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
