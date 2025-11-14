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

    std::string str = "hello";
    root["str"] = str;
    if (str != root["str"].as_string()) {
        std::cerr << "error: " << root["str"].as_string() << std::endl;
        return false;
    }
    std::vector<int> vec_op = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = vec_op;
    json::array vec_obj = vec_op;
    if (vec_obj.as<std::vector<int>>() != root["arr from vec"].as<std::vector<int>>()) {
        std::cerr << "error: " << root["arr from vec"].as_string() << std::endl;
        return false;
    }

    std::map<std::string, int> map_op {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = map_op;
    json::object map_obj = map_op;
    if (map_obj.as<std::map<std::string, int>>() != root["obj from map"].as<std::map<std::string, int>>()) {
        std::cerr << "error: " << root["obj from map"].as_string() << std::endl;
        return false;
    }

    auto jarr = root.get("arr", json::array());
    if (jarr.size() != 7) {
        std::cerr << "error: " << jarr.size() << std::endl;
        return false;
    }

    bool is_int_array = root["arr"].is_array() && root["arr"].all<int>();
    if (!is_int_array) {
        std::cerr << "not all int: " << root["arr"].as_string() << std::endl;
        return false;
    }

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
    json::value to_json(const ThirdPartyStruct& t) const { return t.a; }

    bool check_json(const json::value& j) const { return j.is_number(); }

    bool from_json(const json::value &j, ThirdPartyStruct& out) const
    {
        out.a = j.as_integer();
        return true;
    }
};
}

bool jsonizing()
{
    // then you can use it as json
    ThirdPartyStruct third { 100 };
    ThirdPartyStruct new_third = (ThirdPartyStruct)third;
    if (new_third.a != 100) {
        std::cerr << "error new_third.a: " << new_third.a << std::endl;
        return false;
    }

    struct MyStruct
    {
        std::string str1;
        std::string str2;
        std::string str3;
        std::vector<double> vec;
        std::unordered_map<std::string, std::list<std::map<std::string, std::deque<int>>>> map;
        std::array<int, 5> arr;
        std::variant<bool, std::string, std::array<int, 4>> var;

        enum class W
        {
            A,
            B,
            C,
        } w = W::A;

        MEO_JSONIZATION(str1, str2, str3, vec, map, arr, w, var);
    };

    MyStruct mine;
    mine.str1 = "Hello";
    mine.str2 = "World";
    mine.str3 = "!";
    mine.vec.emplace_back(0.5);
    mine.map = { { "key_1", { { { "inner_key_1", { 7, 8, 9 } } }, { { "inner_key_2", { 10 } } } } } };
    mine.w = MyStruct::W::C;

    json::value j_mine = mine;
    std::cout << j_mine << std::endl;

    MyStruct new_mine = (MyStruct)j_mine;

    bool ret = new_mine.str1 == "Hello" && new_mine.str2 == "World" && new_mine.str3 == "!" && new_mine.vec[0] == 0.5
               && new_mine.map["key_1"].size() == 2 && new_mine.w == MyStruct::W::C;
    if (!ret) {
        std::cerr << "error new_mine" << std::endl;
        return false;
    }

    struct OptTest
    {
        int a = 0;
        int b = 0;

        MEO_JSONIZATION(MEO_OPT a, MEO_OPT b);
    };

    json::value opt_j = json::object();
    if (!opt_j.is<OptTest>()) {
        std::cerr << "bad MEO_OPT" << std::endl;
        return false;
    }

    std::filesystem::path path = "/root/dir1/dir2/filename";
    json::value jpath = path;
    std::filesystem::path new_path = (std::filesystem::path)jpath;
    if (new_path != path) {
        std::cerr << "error new_path: " << new_path << std::endl;
        return false;
    }

    std::vector<std::filesystem::path> paths = { "/root/dir1/dir2/filename", "/root/dir1/dir2/filename2" };
    json::array jpaths = paths;
    std::vector<std::filesystem::path> new_paths = (std::vector<std::filesystem::path>)jpaths;
    if (new_paths != paths) {
        std::cerr << "error new_paths" << std::endl;
        return false;
    }

    std::map<std::string, std::filesystem::path> path_map = { { "key1", "/root/dir1/dir2/filename" },
                                                              { "key2", "/root/dir1/dir2/filename2" } };
    json::object jpath_map = path_map;
    std::map<std::string, std::filesystem::path> new_path_map = (std::map<std::string, std::filesystem::path>)jpath_map;
    if (new_path_map != path_map) {
        std::cerr << "error new_path_map" << std::endl;
        return false;
    }

    std::array<int, 10> stdarr { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    json::value jarr = stdarr;
    std::array<int, 10> new_std_arr;
    if (jarr.is<std::array<int, 10>>()) {
        new_std_arr = (std::array<int, 10>)jarr;
    }
    else {
        std::cerr << "error std::array" << std::endl;
        return false;
    }

    if (new_std_arr.back() != 10) {
        std::cerr << "error std::array value" << std::endl;
        return false;
    }

    if (jarr.is<std::array<int, 5>>()) {
        std::cerr << "error std::array size" << std::endl;
        return false;
    }

    json::array tuple_arr;
    tuple_arr.emplace_back(1);
    tuple_arr.emplace_back("aaabbbccc");
    auto t2 = std::tuple<int, std::string>(tuple_arr);
    auto p2 = std::pair<int, std::string>(tuple_arr);
    json::value tuple_val = tuple_arr;
    auto t3 = std::tuple<int, std::string>(tuple_val);
    auto p3 = std::tuple<int, std::string>(tuple_val);
    bool ist = tuple_val.is<std::tuple<int, std::string>>();
    bool isp = tuple_val.is<std::pair<int, std::string>>();

    auto new_tuple_arr = (json::array)t2;
    auto new_tuple_val = (json::value)t2;
    new_tuple_val.as<std::tuple<int, std::string>>();
    new_tuple_val.as<std::pair<int, std::string>>();

    using VarT = std::variant<bool, std::string, std::array<int, 4>>;
    VarT var = std::string("abc");
    json::value k = var;
    auto var2 = k.as<VarT>();
    bool is_var = k.is<VarT>();
    if (!is_var) {
        std::cerr << "error is std::variant" << std::endl;
        return false;
    }

    auto new_pair_arr = (json::array)p2;
    auto new_pair_val = (json::value)p2;

    json::array movable_arr = { 1, json::array { 2, 3 } };
    auto movable_arr_output = std::move(movable_arr).as<std::tuple<int, json::array>>();
    if (!movable_arr.empty()) {
        std::cerr << "not moved" << std::endl;
        return false;
    }

    return true;
}
