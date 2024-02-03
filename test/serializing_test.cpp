#include <array>
#include <deque>
#include <filesystem>
#include <iostream>
#include <list>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "json.hpp"
#include "serializing_test.h"

bool serializing()
{
    json::value root;

    root["hello"] = "meojson";
    root["Pi"] = 3.1416;
    root["null"] = nullptr;

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

    bool is_int_array = root["arr"].is_array() && root["arr"].all<int>();
    if (!is_int_array) {
        std::cerr << "not all int: " << root["arr"].as_string() << std::endl;
        return false;
    }
    auto to_vec = root["arr"].as_collection<int>();
    auto to_vec_2 = root["arr"].as_collection<int, std::vector>();
    auto to_list = root["arr"].as_collection<int, std::list>();
    auto to_set = root["arr"].as_collection<int, std::set>();
    auto to_hashset = root["arr"].as_collection<int, std::unordered_set>();
    auto to_deque = root["arr"].as_collection<int, std::deque>();
    auto to_q = root["arr"].as_collection<int, std::queue>();

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = vec;

    std::set<std::string> set = { "a", "bb\n\nb", "cc\t" };
    root["arr from set"] = set;

    std::map<std::string, int> map {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = map;

    bool is_int_map = root["obj from map"].is_object() && root["obj from map"].all<int>();
    if (!is_int_map) {
        std::cerr << "not all int: " << root["obj from map"].as_string() << std::endl;
        return false;
    }
    auto as_map = root["obj from map"].as_map<int>();
    auto as_map_2 = root["obj from map"].as_map<int, std::map>();
    auto to_hashmap = root["obj from map"].as_map<int, std::unordered_map>();

    std::vector<std::list<std::set<int>>> complex { { { 1, 2, 3 }, { 4, 5 } }, { { 6 }, { 7, 8 } } };
    root["complex"] = complex;

    root["a\\n"] = "1a\\n";
    root["a\n"] = "2a\n";
    if (root["a\\n"].as_string() != "1a\\n") {
        std::cerr << "error: " << root["a\\n"].as_string() << std::endl;
        return false;
    }
    if (root["a\n"].as_string() != "2a\n") {
        std::cerr << "error: " << root["a\n"].as_string() << std::endl;
        return false;
    }
    if (root["a\\n"].to_string() != "\"1a\\\\n\"") {
        std::cerr << "error: " << root["a\\n"].to_string() << std::endl;
        return false;
    }
    if (root["a\n"].to_string() != "\"2a\\n\"") {
        std::cerr << "error: " << root["a\n"].to_string() << std::endl;
        return false;
    }

    std::cout << root << std::endl;
    std::ofstream ofs("serializing.json");
    ofs << root;
    ofs.close();

    return true;
}

bool wstring_serializing()
{
    json::wvalue root;

    root[L"hello"] = L"meojson";
    root[L"Pi"] = 3.1416;

    root[L"obj"] = {
        { L"obj_key1", L"Hi" },
        { L"obj_key2", 123 },
        { L"obj_key3", true },
    };
    root[L"obj"].emplace(L"obj_key4", 789);

    root[L"obj"].emplace(L"obj_key5", json::wobject { { L"key4 child", L"i am object value" } });
    root[L"another_obj"][L"child"][L"grand"] = L"i am grand";

    // take union
    root[L"obj"] |= json::wobject {
        { L"obj_key6", L"i am string" },
        { L"obj_key7", json::warray { L"i", L"am", L"array" } },
    };

    root[L"arr"] = json::warray { 1, 2, 3 };
    root[L"arr"].emplace(4);
    root[L"arr"].emplace(5);
    root[L"arr"] += json::warray { 6, 7 };

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root[L"arr from vec"] = vec;

    std::set<std::wstring> set = { L"a", L"bb\n\nb", L"cc\t" };
    root[L"arr from set"] = set;

    std::map<std::wstring, int> map {
        { L"key1", 1 },
        { L"key2", 2 },
    };
    root[L"obj from map"] = map;

    root[L"a\\n"] = L"1a\\n";
    root[L"a\n"] = L"2a\n";
    if (root[L"a\\n"].as_string() != L"1a\\n") {
        std::wcerr << "error: " << root[L"a\\n"].as_string() << std::endl;
        return false;
    }
    if (root[L"a\n"].as_string() != L"2a\n") {
        std::wcerr << "error: " << root[L"a\n"].as_string() << std::endl;
        return false;
    }
    if (root[L"a\\n"].to_string() != L"\"1a\\\\n\"") {
        std::wcerr << "error: " << root[L"a\\n"].to_string() << std::endl;
        return false;
    }
    if (root[L"a\n"].to_string() != L"\"2a\\n\"") {
        std::wcerr << "error: " << root[L"a\n"].to_string() << std::endl;
        return false;
    }

    std::wcout << root << std::endl;
    std::wofstream ofs("wstring_serializing.json");
    ofs << root;
    ofs.close();

    return true;
}

struct ThirdPartyStruct
{
    int a = 0;
};

namespace json::ext
{
template <>
class jsonization<ThirdPartyStruct>
{
public:
    json::wvalue to_json(const ThirdPartyStruct& t) const { return t.a; }
    bool check_json(const json::wvalue& j) const { return j.is_number(); }
    bool from_json(const json::wvalue& j, ThirdPartyStruct& out) const
    {
        out.a = j.as_integer();
        return true;
    }
};
} // namespace json::ext

bool jsonizing()
{
    // then you can use it as json
    ThirdPartyStruct third { 100 };
    json::wvalue jthird = third;
    ThirdPartyStruct new_third = (ThirdPartyStruct)jthird;

    json::warray arr = { third, new_third };
    json::wobject obj = { { L"third", third }, { L"new_third", new_third } };

    struct MyStruct
    {
        std::string str1;
        std::string str2;
        std::string str3;
        std::vector<double> vec;
        std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>> map;

        MEO_JSONIZATION(str1, str2, str3, vec, map);
    };

    MyStruct mine;
    mine.str1 = "Hello";
    mine.str2 = "World";
    mine.str3 = "!";
    mine.vec.emplace_back(0.5);
    mine.map = { { "key_1", { { { "inner_key_1", { 7, 8, 9 } } }, { { "inner_key_2", { 10 } } } } } };

    json::value j_mine = mine;
    MyStruct new_mine = (MyStruct)j_mine;

    return new_third.a == 100 && new_mine.str1 == "Hello" && new_mine.str2 == "World" && new_mine.str3 == "!" &&
           new_mine.vec[0] == 0.5 && new_mine.map["key_1"].size() == 2;
}
