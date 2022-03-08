<div align="center">

# meojson

现代化的全平台 Json/Json5 解析/生成器，Header-only，并附带大量语法糖！

A modern all-platform Json/Json5 parser/serializer, which is header-only and contains lots of syntactic sugar!

</div>

[English](./README_en.md)

## 使用说明

- 在您的项目中包含头文件即可使用  

```cpp
#include "json.hpp"
```

- 若您需要解析 Json5, 则请包含 `json5.hpp` 头文件

```cpp
#include "json5.hpp"
```

- meojson 仅依赖 STL, 但需要 c++17 标准

## 示例

### 解析

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
        1,
        2,
        3
    ],
    "str": "abc",
    "num": 3.1416,
    "A_obj": {
        "B_arr": [
            {
                "C_str": "you found me!"
            }
        ]
    }
}
    )";

    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    auto value = ret.value();  // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`
    // Output: meojson
    std::cout << value["repo"].as_string() << std::endl;

    /* Output:
        ChingCdesu's homepage: https://github.com/ChingCdesu
        MistEO's homepage: https://github.com/MistEO
    */
    for (auto&& [name, homepage] : value["author"].as_object()) {
        std::cout << name << "'s homepage: " << homepage.as_string() << std::endl;
    }

    // Output: abc
    std::string str = (std::string)value["str"];    // it is equivalent to `value["str"].as_string()`
    std::cout << str << std::endl;

    // Output: 3.141600
    double num = value["num"].as_double();          // similarly, you can use `(double)value["num"]`
    std::cout << num << std::endl;

    // Output: default_value
    std::string get = value.get("maybe_exists", "default_value");
    std::cout << get << std::endl;

    // Output: you found me!
    std::string nested_get = value.get("A_obj", "B_arr", 0, "C_str", "default_value");
    std::cout << nested_get << std::endl;

    /*  Output:
        1
        2
        3
    */
    // It's const!
    for (const auto& ele : value.at("list").as_array()) {
        int x = (int)ele;
        std::cout << x << std::endl;
    }
}
```

### 解析 Json5

```cpp
/***
 * from sample/json5_parse.cpp
***/
#include <iostream>
#include "json5.hpp"

void parsing()
{
    std::string content = R"(
// 这是一段json5格式的信息
{
  名字: "MistEO",                  /* key的引号可省略 */
  😊: '😄',                       // emoji为key
  thanks: 'ありがとう',             /* 单引号也可以表示字符串 */
  \u006Bey: ['value',],            // 普通字符和转义可以混用
  inf: +Infinity, nan: NaN,        // 数字可以以"+"开头
  fractional: .3, integer: 42.,    // 小数点作为起始/结尾
  byte_max: 0xff,                  // 十六进制数
  light_speed: +3e8,               // 科学计数法
}
)";
    auto ret = json::parse5(content);
    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    auto value = ret.value();  // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`

    // Output: MistEO
    std::cout << value["名字"] << std::endl;
    // Output: value
    std::string str = (std::string)value["key"][0];
    std::cout << str << std::endl;
    
    // for more json::value usage, please refer to sample.cpp
}
```

### 生成

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void serializing()
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

    auto other = json::object{
        { "other_key", "lol" },
        { "obj", "Existing key will not be overwritten"}
    };
    // take union
    root |= other;

    std::cout << root.format() << std::endl;
}
```
