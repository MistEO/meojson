# meojson

[English](#CPP_JSON_Parser_Generator)

## C++ JSON解析/生成库

### 简介

- `meojson`是一个`非常快`的C++ JSON解析库，O(n)级复杂度，解析速度比腾讯RapidJson快70%以上！

    ```bash
    # 测试方法
    cd test/parse/
    chmod a+x performance_test.sh
    ./performance_test.sh
    ```

    ![image](test/parse/performance_result.png)

- `meojson`也是一个使用方便的C++ JSON生成库，可以使用各种语法糖优雅地生成JSON

    ```c++
    json::value json;
    json["hello"] = "meojson";
    json["Pi"] = 3.1416;

    json["arr"] = json::array(
        {"a", "b", "c"}
    );

    json["obj"] = json::object(
        {
            {"obj_key1", "aaa"},
            {"obj_key2", 123},
            {"obj_key3", true}
        }
    );
    json["obj_another"]["child"]["grand"] = "i am grand";

    // Output "{"obj_another":{"child":{"grand":"i am grand"}},"obj":{"obj_key3":true,"obj_key2":123,"obj_key1":"aaa"},"arr":["a","b","c"],"Pi":3.141600,"hello":"meojson"}"
    std::cout << json.to_string() << std::endl;
    ```

- `meojson`完全符合JSON标准

    ```bash
    # 测试方法
    cd test/parse/
    chmod a+x conformance_test.sh
    ./conformance_test.sh
    ```

    ![image](test/parse/conformance_result_1.png)
    ![image](test/parse/conformance_result_2.png)
    其中`fail18.json`并不是一个错误的JSON，可以参考[这个链接](https://stackoverflow.com/questions/42116718/is-there-an-array-depth-limitation-in-json)

    当然如果出现未发现的解析错误，也欢迎向我提ISSUE

- `meojson`仅依赖于STL，但需要编译器支持C++17标准

### 安装

1. 构建动态库并安装

    ```bash
    make release
    sudo make install
    ```

2. 在您的项目，添加`-lmeojson`编译选项
3. 集成代码并使用

### 示例

```c++
/***
 * from sample/sample.cpp
***/

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
        json["hello"] = "meojson";
        json["Pi"] = 3.1416;

        json["arr"] = json::array(
            {"a", "b", "c"});
        json["obj"] = json::object(
            {{"obj_key1", "aaa"},
             {"obj_key2", 123},
             {"obj_key3", true}});
        json["obj_another"]["child"]["grand"] = "i am grand";

        // Output "{"obj_another":{"child":{"grand":"i am grand"}},"obj":{"obj_key3":true,"obj_key2":123,"obj_key1":"aaa"},"arr":["a","b","c"],"Pi":3.141600,"hello":"meojson"}"
        std::cout << json.to_string() << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
```

## CPP_JSON_Parser_Generator

### Introduction

- `meojson` is a `very fast` json parser for C++, O(n) Complexity, 70% faster than Tecent's RapidJson.

    ```bash
    # How to test
    cd test/parse/
    chmod a+x performance_test.sh
    ./performance_test.sh
    ```

    ![image](test/parse/performance_result.png)

- `meojson` is also an easy-to-use json generator for C++ that can use various syntactic sugars to elegantly generate JSON

    ```c++
    json::value json;
    json["hello"] = "meojson";
    json["Pi"] = 3.1416;

    json["arr"] = json::array(
        {"a", "b", "c"}
    );

    json["obj"] = json::object(
        {
            {"obj_key1", "aaa"},
            {"obj_key2", 123},
            {"obj_key3", true}
        }
    );
    json["obj_another"]["child"]["grand"] = "i am grand";

    // Output "{"obj_another":{"child":{"grand":"i am grand"}},"obj":{"obj_key3":true,"obj_key2":123,"obj_key1":"aaa"},"arr":["a","b","c"],"Pi":3.141600,"hello":"meojson"}"
    std::cout << json.to_string() << std::endl;
    ```

- `meojson` is fully compliant with the JSON standard

    ```bash
    # How to test
    cd test/parse/
    chmod a+x conformance_test.sh
    ./conformance_test.sh
    ```

    ![image](test/parse/conformance_result_1.png)
    ![image](test/parse/conformance_result_2.png)
    Among them, `fail18.json` is not a wrong JSON, you can refer to [this link](https://stackoverflow.com/questions/42116718/is-there-an-array-depth-limitation-in-json)

    Of course, if there is an undiscovered parsing error, you are also welcome to mention ISSUE to me

- `meojson` only depends on STL, but requires the compiler to support the C++17 standard

### Installation

1. Build dynamic library and install

    ```bash
    make release
    sudo make install
    ```

2. Add `-lmeojson` compilation option to your project
3. Integrate code and enjoy it!

### Sample

```c++
/***
 * from sample/sample.cpp
***/

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
        json["hello"] = "meojson";
        json["Pi"] = 3.1416;

        json["arr"] = json::array(
            {"a", "b", "c"});
        json["obj"] = json::object(
            {{"obj_key1", "aaa"},
             {"obj_key2", 123},
             {"obj_key3", true}});
        json["obj_another"]["child"]["grand"] = "i am grand";

        // Output "{"obj_another":{"child":{"grand":"i am grand"}},"obj":{"obj_key3":true,"obj_key2":123,"obj_key1":"aaa"},"arr":["a","b","c"],"Pi":3.141600,"hello":"meojson"}"
        std::cout << json.to_string() << std::endl;
    }

    std::cout << std::endl;

    return 0;
}
```
