#include <iostream>

#include "json.h"

int main()
{

    /*** Parse ***/
    {
        std::cout << "****** Parsing ****** " << std::endl;
        std::string content = R"(
        {
            "prject": {
                "name": "meojson",
                "list": [ 
                    1, 2, 3
                ]
            }
        }
        )";
        //std::cout << content << std::endl;

        auto &&[ret, value] = json::parser::parse(content);

        if (ret)
        {
            // Output "meojson"
            std::cout << value["prject"]["name"].as_string() << std::endl;
            // Output 2
            std::cout << value["prject"]["list"][1].as_integer() << std::endl;
        }
        else
        {
            std::cout << "parse failed" << std::endl;
        }
    }

    std::cout << std::endl;

    /*** Generate ***/
    {
        std::cout << "****** Generating ******" << std::endl;

        json::value json;
        json["lib"] = "meojson";
        json["Pi"] = 3.1416;
        json["stars"] = 0;
        json["stars"] = json["stars"].as_integer() + 1;

        json["arr"] = json::array(
            {"a", "b", "c"});

        json["obj_aaa"] = json::object(
            {{"key1", "value1"},
             {"key2", 123}});

        json["obj_bbb"]["key3"] = json::array(
            {1, 2, 3});

        // Output "{"obj_bbb":{"key3":[1,2,3]},"obj_aaa":{"key2":123,"key1":"value1"},"arr":["a","b","c"],"Pi":3.141600,"stars":1,"lib":"meojson"}"
        std::cout << json.to_string() << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
