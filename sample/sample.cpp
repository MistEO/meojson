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
