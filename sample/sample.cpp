#include <iostream>

#include "json.h"

int main()
{

    /*** Parse ***/
    {
        std::cout << "****** Parsing ****** " << std::endl;
        std::string content = R"(
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/sample.out",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/build",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "make_debug",
            "number_test": 123.46e-31
        }
    ]
}
)";
        //std::cout << content << std::endl;

        auto &&[ret, json] = json::parser::parse(content);

        if (ret)
        {
            std::cout << "parse success" << std::endl;

            std::cout << json["configurations"][0]["name"].as_string() << std::endl;
            for (auto &&[key, val] : json["configurations"][0]["setupCommands"][0].as_object())
            {
                std::cout << key << ": " << val.to_string() << std::endl;
            }
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

        json::value obj;
        obj["key1"] = "hello";
        obj["PI"] = 3.1416;
        obj["list"] = json::array({"a", "b", "c"});

        obj["child"]["obj2_key1"] = "i am child";

        std::cout << obj.to_string() << std::endl;
    }

    return 0;
}
