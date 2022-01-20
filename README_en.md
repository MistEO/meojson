<div align="center">

# meojson

现代化的全平台 Json/Json5 解析/生成器，Header-only, 并附带大量语法糖！

A modern all-platform Json/Json5 parser/generator, with Header-only and lots of syntactic sugar!

</div>

[中文](./README.md)

## Usage

- Include the header file in your project, and enjoy it!  

```cpp
#include "json.hpp"
```

- If you need to parse Json5, please include `json5.hpp`

```cpp
#include "json5.hpp"
```

- meojson only depends on STL, but requires c++17 standard

## Sample

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>

#include "json.hpp"

int main()
{
    std::cout << "*** 解析 | Parsing ***" << std::endl;

    std::string content = R"(
    {
        "repo": "meojson",
        "author": {
            "MistEO": "https://github.com/MistEO",
            "ChingCdesu": "https://github.com/ChingCdesu"
        },
        "list": [
            1, 2, 3
        ]
    }
    )";

    auto ret = json::parser::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return -1;
    }
    auto value = ret.value();  // As also, you can use rvalues, like  
                               // `auto value = std::move(ret).value();`
    // Output: meojson
    std::cout << value["repo"].as_string() << std::endl;

    /* Output:
        ChingCdesu 's homepage: "https://github.com/ChingCdesu"
        MistEO 's homepage: "https://github.com/MistEO"
    */
    for (auto&& [name, homepage] : value["author"].as_object()) {
        std::cout << name << " 's homepage: " << homepage << std::endl;
    }

    std::string str = value.get("not_exists", "not found");

    /*  Output:
        1
        2
        3
    */
    // It's const!
    for (const auto& num : value.at("list").as_array()) {
        int x = num.as_integer();
    }

    std::cout << "*** 生成 | Generating ***" << std::endl;

    json::value root;
    root["hello"] = "meojson";
    root["Pi"] = 3.1416;

    root["arr"] = json::array{
        "a", "b", "c"
    };
    root["obj"] = json::object{
        {"obj_key1", "aaa"},
        {"obj_key2", 123},
        {"obj_key3", true}
    };
    root["obj"].as_object().emplace("key4", json::object{ { "key4 child", "lol" } });
    root["obj_another"]["child"]["grand"] = "i am grand";

    std::cout << root.format() << std::endl;

    return 0;
}


```
