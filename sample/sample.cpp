#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "json.hpp"

bool parsing();
bool parsing_width();
bool serializing();
void test_jsonization();

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

    test_jsonization();

    return 0;
}

bool parsing()
{
    std::string content = R"(
{
    "repo": "meojson",
    "author": {
        "MistEO": "https://github.com/MistEO",
        "ChingCdesu": "https://github.com/ChingCdesu"
    },
    "list": [ 1, 2, 3 ],
    "str": "abc\n123",
    "num": 3.1416,
    "A_obj": {
        "B_arr": [
            { "C_str": "i am a distraction" },
            { "C_str": "you found me!" }
        ]
    }
}
    )";

    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return false;
    }
    json::value& value = ret.value(); // you can use rvalues if needed, like
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
    std::string str = (std::string)value["str"]; // it is equivalent to `value["str"].as_string()`
    std::cout << str << std::endl;

    // Output: 3.141600
    double num = value["num"].as_double(); // similarly, you can use `(double)value["num"]`
    std::cout << num << std::endl;

    // Output: default_value
    std::string get = value.get("maybe_exists", "default_value");
    std::cout << get << std::endl;

    // Output: you found me!
    std::string nested_get = value.get("A_obj", "B_arr", 1, "C_str", "default_value");
    std::cout << nested_get << std::endl;

    // Output: 1, 2, 3
    // If the "list" is not an array or not exists, it will be a invalid optional;
    auto opt = value.find<json::array>("list");
    if (opt) {
        auto& arr = opt.value();
        for (auto&& elem : arr) {
            std::cout << elem.as_integer() << std::endl;
        }
    }
    // more examples, it will output 3.141600
    auto opt_n = value.find<double>("num");
    if (opt_n) {
        std::cout << opt_n.value() << std::endl;
    }
    // If you use the `find` without template argument, it will return a `std::optional<json::value>`
    auto opt_v = value.find("not_exists");
    std::cout << "Did we find the \"not_exists\"? " << opt_v.has_value() << std::endl;

    bool is_vec = value["list"].is<std::vector<int>>();
    if (is_vec) {
        std::vector<int> to_vec = value["list"].as_collection<int>();
        to_vec = (std::vector<int>)value["list"];
        to_vec = value["list"].as<std::vector<int>>();

        // Output: 1, 2, 3
        for (auto&& i : to_vec) {
            std::cout << i << std::endl;
        }
    }

    std::list<int> to_list = value["list"].as_collection<int, std::list>();
    to_list = (std::list<int>)value["list"];
    to_list = value["list"].as<std::list<int>>();

    std::set<int> to_set = value["list"].as_collection<int, std::set>();
    to_set = (std::set<int>)value["list"];
    to_set = value["list"].as<std::set<int>>();

    bool is_map = value["author"].is<std::map<std::string, std::string>>();
    if (is_map) {
        std::map<std::string, std::string> to_map = value["author"].as_map<std::string>();
        to_map = (std::map<std::string, std::string>)value["author"];
        to_map = value["author"].as<std::map<std::string, std::string>>();
    }

    auto to_hashmap = value["author"].as_map<std::string, std::unordered_map>();
    to_hashmap = (std::unordered_map<std::string, std::string>)value["author"];
    to_hashmap = value["author"].as<std::unordered_map<std::string, std::string>>();

    using namespace json::literals;
    json::value val = "{\"hi\":\"literals\"}"_json;
    std::cout << val["hi"] << std::endl;

    std::vector<uint16_t> vec_j = { '{', '"', 'k', '"', ':', '"', 'v', '"', '}' };
    auto vecj_opt = json::parse(vec_j);
    if (vecj_opt) {
        std::ignore = vecj_opt->dumps();
    }

    return true;
}

bool parsing_width()
{
    std::wstring_view content = LR"(
{
    "repo": "meojson",
    "author": {
        "MistEO": "https://github.com/MistEO",
        "ChingCdesu": "https://github.com/ChingCdesu"
    },
    "list": [ 1, 2, 3 ],
    "str": "abc\n123",
    "num": 3.1416,
    "A_obj": {
        "B_arr": [
            { "C_str": "i am a distraction" },
            { "C_str": "you found me!" }
        ]
    }
}
    )";

    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return false;
    }
    json::wvalue& value = ret.value(); // you can use rvalues if needed, like
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
    std::wstring str = (std::wstring)value[L"str"]; // it is equivalent to `value["str"].as_string()`
    std::wcout << str << std::endl;

    // Output: 3.141600
    double num = value[L"num"].as_double(); // similarly, you can use `(double)value["num"]`
    std::wcout << num << std::endl;

    // Output: default_value
    std::wstring get = value.get(L"maybe_exists", L"default_value");
    std::wcout << get << std::endl;

    // Output: you found me!
    std::wstring nested_get = value.get(L"A_obj", L"B_arr", 1, L"C_str", L"default_value");
    std::wcout << nested_get << std::endl;

    // Output: 1, 2, 3
    // If the "list" is not an array or not exists, it will be a invalid optional;
    auto opt = value.find<json::warray>(L"list");
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

    std::wcout << value.format(2) << std::endl;

    using namespace json::literals;
    auto val = LR"({"hi":"literals"})"_json;
    std::wcout << val[L"hi"] << std::endl;

    return true;
}

bool serializing()
{
    json::value root;

    root["hello"] = "meojson";
    root["Pi"] = 3.1416;

    root["obj"] = {
        { "obj_key1", "Hi" },
        { "obj_key2", 123 },
        { "obj_key3", true },
    };
    root["obj"].emplace("obj_key4", 789);

    root["obj"].emplace("obj_key5", json::object { { "key4 child", "i am object value" } });
    root["another_obj"]["child"]["grand"] = "i am grand";

    // take union
    root["obj"] |= json::object {
        { "obj_key6", "i am string" },
        { "obj_key7", json::array { "i", "am", "array" } },
    };

    root["arr"] = json::array { 1, 2, 3 };
    root["arr"].emplace(4);
    root["arr"].emplace(5);
    root["arr"] += json::array { 6, 7 };

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = vec;

    std::set<std::string> set = { "a", "bb\n\nb", "cc\t" };
    root["arr from set"] = set;

    std::map<std::string, int> map {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = map;

    std::vector<std::list<std::set<int>>> complex { { { 1, 2, 3 }, { 4, 5 } }, { { 6 }, { 7, 8 } } };
    root["complex"] = json::serialize<false>(complex);

    std::map<std::vector<std::string>, std::map<int, std::vector<double>>> more_complex {
        { { "i", "am", "key1" }, { { 1, { 0.1, 0.2 } }, { 2, { 0.2, 0.3 } } } },
        { { "key2" }, { { 3, { 0.4 } }, { 4, { 0.5, 0.6, 0.7 } } } },
    };
    // the "std::map" cannot be converted to json because the key is "std::vector<std::string>",
    // you can set the template parameter "loose" of "serialize" to true, which will make a more relaxed conversion.
    root["more_complex"] = json::serialize<true>(more_complex);

    // for test
    root["a\\n"] = "1a\\n";
    root["a\n"] = "2a\n";

    std::cout << root << std::endl;

    std::ofstream ofs("meo.json");
    ofs << root;
    ofs.close();

    return true;
}

void test_jsonization()
{
    struct MyStruct
    {
        std::vector<int> vec;
        std::map<std::string, int> map;

        int i = 0;
        double d = 0;

        // if you are using MSVC, please add "/Zc:preprocessor" to your project
        MEO_JSONIZATION(vec, map, i, d);
    };

    MyStruct a;
    a.vec = { 1, 2, 3 };
    a.map = { { "key", 5 } };
    a.i = 100;
    a.d = 0.5;

    json::object j = a.dump_to_json();
    std::cout << j << std::endl;

    MyStruct b;
    b.load_from_json(j);

    std::cout << b.dump_to_json() << std::endl;
}
