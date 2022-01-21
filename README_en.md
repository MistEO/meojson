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

### Parsing

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void parsing()
{
    std::string content = R"(
    {
        "repo": "meojson",
        "author": {
            "MistEO": "https://github.com/MistEO",
            "ChingCdesu": "https://github.com/ChingCdesu"
        },
        "list": [
            1, 2, 3
        ],
        "str": "abc",
        "num": 3.1416
    }
    )";

    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    auto value = ret.value();  // As also, you can use rvalues, like  
                               // `auto value = std::move(ret).value();`
    // Output: meojson
    std::cout << value["repo"] << std::endl;

    /* Output:
        ChingCdesu 's homepage: "https://github.com/ChingCdesu"
        MistEO 's homepage: "https://github.com/MistEO"
    */
    for (auto&& [name, homepage] : value["author"].as_object()) {
        std::cout << name << " 's homepage: " << homepage << std::endl;
    }

    // Output: abc
    std::string str = (std::string)value["str"];    // As also, you can use `value["str"].as_string()`
    std::cout << str << std::endl;

    // Output: 3.141600
    double num = value["num"].as_double();          // As also, you can use `(double)value["num"]`
    std::cout << num << std::endl;

    // Output: not found
    std::string str_get = value.get("maybe_exists", "not found");
    std::cout << str_get << std::endl;

    /*  Output:
        1
        2
        3
    */
    // It's const!
    for (const auto& num : value.at("list").as_array()) {
        int x = num.as_integer();
        std::cout << x << std::endl;
    }
}
```

### Generating

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void generating()
{
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
    root["obj"].object_emplace("key4", json::object{ { "key4 child", "lol" } });
    root["obj_another"]["child"]["grand"] = "i am grand";

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = json::array(vec);
    root["arr from vec"].array_emplace(6);
    
    std::set<std::string> set = { "a", "bbb", "cc" };
    root["arr from set"] = json::array(set);

    std::map<std::string, int> map;
    map.emplace("key1", 1);
    map.emplace("key2", 2);
    root["obj from map"] = json::object(map);

    std::cout << root.format() << std::endl;
}
```
