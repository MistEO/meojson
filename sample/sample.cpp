#include <iostream>
#include <set>
#include <vector>
#include <map>

#include "json.hpp"

bool parsing();
bool serializing();

int main()
{
    std::cout << "\n****** Parsing ******\n" << std::endl;

    if (!parsing()) {
        return -1;
    }

    std::cout << "\n****** Serializing ******\n" << std::endl;

    if (!serializing()) {
        return -1;
    }

    return 0;
}

bool parsing()
{
    std::wstring_view content = LR"(
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

    auto ret = json::parse<std::wstring_view, std::wstring>(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return false;
    }
    auto& value = ret.value(); // you can use rvalues if needed, like
    // `auto value = std::move(ret).value();`
// Output: meojson
    std::wcout << value[L"repo"].as_string() << std::endl;

    /* Output:
        ChingCdesu's homepage: https://github.com/ChingCdesu
        MistEO's homepage: https://github.com/MistEO
    */
    for (auto&& [name, homepage] : value[L"author"].as_object()) {
        std::wcout << name << "'s homepage: " << homepage.as_string() << std::endl;
    }

    // Output: abc
    std::wstring str = (std::wstring)value[L"str"];    // it is equivalent to `value["str"].as_string()`
    std::wcout << str << std::endl;

    // Output: 3.141600
    double num = value[L"num"].as_double();          // similarly, you can use `(double)value["num"]`
    std::wcout << num << std::endl;

    // Output: default_value
    std::wstring get = value.get(L"maybe_exists", L"default_value");
    std::wcout << get << std::endl;

    // Output: you found me!
    std::wstring nested_get = value.get(L"A_obj", L"B_arr", 0, L"C_str", L"default_value");
    std::wcout << nested_get << std::endl;

    // Output: 1, 2, 3
    // If the "list" is not an array or not exists, it will be a invalid optional;
    auto opt = value.find<json::array>(L"list");
    if (opt) {
        auto& arr = opt.value();
        for (auto&& elem : arr) {
            std::cout << elem.as_integer() << std::endl;
        }
    }
    // more examples, it will output 3.141600
    auto opt_n = value.find<double>(L"num");
    if (opt_n) {
        std::cout << opt_n.value() << std::endl;
    }
    // If you use the `find` without template argument, it will return a `std::optional<json::value>`
    auto opt_v = value.find(L"not_exists");
    std::cout << "Did we find the \"not_exists\"? " << opt_v.has_value() << std::endl;

    return true;
}

bool serializing()
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

    root["arr from vec"] += json::array{ 7, 8, 9, 10 };

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

    std::cout << root.format(true) << std::endl;

    // test operator==()
    std::cout << "\n****** sub test value equal ******\n" << std::endl;

    json::value root_copy = root; // copy value `root`

    std::cout << "before: root_copy " << (root_copy == root ? "==" : "!=") << " root" << std::endl;

    root_copy["hello"] = "windsgo hello"; // revise a string
    root_copy["arr"].as_array()[2] = "B"; // revise an array element

    std::cout << "after : root_copy " << (root_copy == root ? "==" : "!=") << " root" << std::endl;

    return true;
}
