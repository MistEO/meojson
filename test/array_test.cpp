#include <algorithm>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <vector>

#include "json.hpp"
#include "array_test.h"

bool test_array_constructors();
bool test_array_access_methods();
bool test_array_modification();
bool test_array_iterators();
bool test_array_operators();
bool test_array_serialization();

bool array_test()
{
    return test_array_constructors() && test_array_access_methods() && test_array_modification() && test_array_iterators()
           && test_array_operators() && test_array_serialization();
}

bool test_array_constructors()
{
    std::cout << "Testing array constructors..." << std::endl;

    // 默认构造
    json::array arr1;
    if (!arr1.empty() || arr1.size() != 0) {
        std::cerr << "Default constructor test failed" << std::endl;
        return false;
    }

    // 初始化列表构造
    json::array arr2 { 1, 2, "three", true, 5.5 };
    if (arr2.size() != 5) {
        std::cerr << "Initializer list constructor test failed" << std::endl;
        return false;
    }

    // 指定大小构造
    json::array arr3(10);
    if (arr3.size() != 10) {
        std::cerr << "Size constructor test failed" << std::endl;
        return false;
    }

    // 拷贝构造
    json::array arr4 = arr2;
    if (arr4.size() != 5) {
        std::cerr << "Copy constructor test failed" << std::endl;
        return false;
    }

    // 移动构造
    json::array arr5 = std::move(arr4);
    if (arr5.size() != 5) {
        std::cerr << "Move constructor test failed" << std::endl;
        return false;
    }

    // 从std::vector构造
    std::vector<int> vec { 1, 2, 3, 4, 5 };
    json::array arr6 = vec;
    if (arr6.size() != 5 || arr6[0].as_integer() != 1) {
        std::cerr << "Constructor from std::vector test failed" << std::endl;
        return false;
    }

    // 从std::list构造
    std::list<std::string> lst { "a", "b", "c" };
    json::array arr7 = lst;
    if (arr7.size() != 3 || arr7[1].as_string() != "b") {
        std::cerr << "Constructor from std::list test failed" << std::endl;
        return false;
    }

    // 从std::set构造
    std::set<int> s { 3, 1, 2 };
    json::array arr8 = s;
    if (arr8.size() != 3) {
        std::cerr << "Constructor from std::set test failed" << std::endl;
        return false;
    }

    std::cout << "Array constructors test passed" << std::endl;
    return true;
}

bool test_array_access_methods()
{
    std::cout << "Testing array access methods..." << std::endl;

    json::array arr { 1, 2, "three", 4.5, true, nullptr };

    // empty和size测试
    if (arr.empty() || arr.size() != 6) {
        std::cerr << "empty() or size() test failed" << std::endl;
        return false;
    }

    // contains测试
    if (!arr.contains(0) || !arr.contains(5) || arr.contains(6)) {
        std::cerr << "contains() test failed" << std::endl;
        return false;
    }

    // exists测试
    if (!arr.exists(2) || arr.exists(10)) {
        std::cerr << "exists() test failed" << std::endl;
        return false;
    }

    // at测试
    if (arr.at(0).as_integer() != 1 || arr.at(2).as_string() != "three") {
        std::cerr << "at() test failed" << std::endl;
        return false;
    }

    // operator[]测试
    if (arr[1].as_integer() != 2 || arr[4].as_boolean() != true) {
        std::cerr << "operator[] test failed" << std::endl;
        return false;
    }

    // get测试（带默认值）
    int val = arr.get(0, 999);
    if (val != 1) {
        std::cerr << "get() test failed for existing index" << std::endl;
        return false;
    }

    int default_val = arr.get(100, 999);
    if (default_val != 999) {
        std::cerr << "get() test failed for non-existing index with default" << std::endl;
        return false;
    }

    // find测试
    auto found = arr.find<int>(0);
    if (!found.has_value() || found.value() != 1) {
        std::cerr << "find() test failed for existing index" << std::endl;
        return false;
    }

    auto not_found = arr.find(20);
    if (not_found.has_value()) {
        std::cerr << "find() test failed for non-existing index" << std::endl;
        return false;
    }

    // all<T>测试
    json::array int_arr { 1, 2, 3, 4, 5 };
    if (!int_arr.all<int>()) {
        std::cerr << "all<int>() test failed for int array" << std::endl;
        return false;
    }

    json::array mixed_arr { 1, "two", 3 };
    if (mixed_arr.all<int>()) {
        std::cerr << "all<int>() test failed for mixed array" << std::endl;
        return false;
    }

    std::cout << "Array access methods test passed" << std::endl;
    return true;
}

bool test_array_modification()
{
    std::cout << "Testing array modification..." << std::endl;

    json::array arr;

    // emplace_back测试
    arr.emplace_back(1);
    arr.emplace_back("two");
    arr.emplace_back(3.3);
    if (arr.size() != 3 || arr[1].as_string() != "two") {
        std::cerr << "emplace_back() test failed" << std::endl;
        return false;
    }

    // push_back测试
    arr.push_back(json::value(true));
    arr.push_back(json::value(nullptr));
    if (arr.size() != 5 || !arr[3].as_boolean()) {
        std::cerr << "push_back() test failed" << std::endl;
        return false;
    }

    // operator[]修改测试
    arr[0] = 100;
    if (arr[0].as_integer() != 100) {
        std::cerr << "operator[] modification test failed" << std::endl;
        return false;
    }

    // erase测试（通过索引）
    json::array arr2 { 1, 2, 3, 4, 5 };
    bool erased = arr2.erase(2);
    if (!erased || arr2.size() != 4 || arr2[2].as_integer() != 4) {
        std::cerr << "erase(index) test failed" << std::endl;
        return false;
    }

    bool not_erased = arr2.erase(10);
    if (not_erased) {
        std::cerr << "erase() should return false for invalid index" << std::endl;
        return false;
    }

    // erase测试（通过迭代器）
    json::array arr3 { 10, 20, 30 };
    auto it = arr3.begin() + 1;
    bool erased_by_iter = arr3.erase(it);
    if (!erased_by_iter || arr3.size() != 2 || arr3[1].as_integer() != 30) {
        std::cerr << "erase(iterator) test failed" << std::endl;
        return false;
    }

    // clear测试
    arr.clear();
    if (!arr.empty() || arr.size() != 0) {
        std::cerr << "clear() test failed" << std::endl;
        return false;
    }

    std::cout << "Array modification test passed" << std::endl;
    return true;
}

bool test_array_iterators()
{
    std::cout << "Testing array iterators..." << std::endl;

    json::array arr { 1, 2, 3, 4, 5 };

    // begin/end测试
    int count = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        count++;
    }
    if (count != 5) {
        std::cerr << "begin()/end() iterator test failed" << std::endl;
        return false;
    }

    // cbegin/cend测试
    count = 0;
    for (auto it = arr.cbegin(); it != arr.cend(); ++it) {
        count++;
    }
    if (count != 5) {
        std::cerr << "cbegin()/cend() iterator test failed" << std::endl;
        return false;
    }

    // rbegin/rend测试（反向迭代器）
    count = 0;
    int expected_val = 5;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
        if (it->as_integer() != expected_val) {
            std::cerr << "rbegin()/rend() value test failed" << std::endl;
            return false;
        }
        expected_val--;
        count++;
    }
    if (count != 5) {
        std::cerr << "rbegin()/rend() iterator test failed" << std::endl;
        return false;
    }

    // crbegin/crend测试
    count = 0;
    for (auto it = arr.crbegin(); it != arr.crend(); ++it) {
        count++;
    }
    if (count != 5) {
        std::cerr << "crbegin()/crend() iterator test failed" << std::endl;
        return false;
    }

    // 范围for循环测试
    count = 0;
    int sum = 0;
    for (const auto& val : arr) {
        count++;
        sum += val.as_integer();
    }
    if (count != 5 || sum != 15) {
        std::cerr << "Range-based for loop test failed" << std::endl;
        return false;
    }

    // 修改测试
    for (auto& val : arr) {
        if (val.is_number()) {
            val = val.as_integer() * 2;
        }
    }
    if (arr[0].as_integer() != 2 || arr[4].as_integer() != 10) {
        std::cerr << "Iterator modification test failed" << std::endl;
        return false;
    }

    // std::算法测试
    json::array arr2 { 5, 2, 8, 1, 9 };
    auto max_it = std::max_element(arr2.begin(), arr2.end(), [](const json::value& a, const json::value& b) {
        return a.as_integer() < b.as_integer();
    });
    if (max_it == arr2.end() || max_it->as_integer() != 9) {
        std::cerr << "std::max_element test failed" << std::endl;
        return false;
    }

    std::cout << "Array iterators test passed" << std::endl;
    return true;
}

bool test_array_operators()
{
    std::cout << "Testing array operators..." << std::endl;

    // operator==测试
    json::array arr1 { 1, 2, 3 };
    json::array arr2 { 1, 2, 3 };
    json::array arr3 { 1, 2, 4 };

    if (!(arr1 == arr2)) {
        std::cerr << "operator== test failed for equal arrays" << std::endl;
        return false;
    }
    if (arr1 == arr3) {
        std::cerr << "operator== test failed for different arrays" << std::endl;
        return false;
    }

    // operator!=测试
    if (arr1 != arr2) {
        std::cerr << "operator!= test failed for equal arrays" << std::endl;
        return false;
    }
    if (!(arr1 != arr3)) {
        std::cerr << "operator!= test failed for different arrays" << std::endl;
        return false;
    }

    // operator+测试（拼接）
    json::array arr4 { 1, 2 };
    json::array arr5 { 3, 4 };
    json::array arr6 = arr4 + arr5;

    if (arr6.size() != 4 || arr6[0].as_integer() != 1 || arr6[3].as_integer() != 4) {
        std::cerr << "operator+ test failed" << std::endl;
        return false;
    }

    // operator+=测试
    json::array arr7 { 1, 2 };
    arr7 += json::array { 3, 4, 5 };

    if (arr7.size() != 5 || arr7[2].as_integer() != 3 || arr7[4].as_integer() != 5) {
        std::cerr << "operator+= test failed" << std::endl;
        return false;
    }

    // operator=测试
    json::array arr8;
    arr8 = arr1;
    if (arr8.size() != 3 || arr8[1].as_integer() != 2) {
        std::cerr << "operator= test failed" << std::endl;
        return false;
    }

    std::cout << "Array operators test passed" << std::endl;
    return true;
}

bool test_array_serialization()
{
    std::cout << "Testing array serialization..." << std::endl;

    json::array arr { 1, "two", 3.14, true, nullptr, json::array { "nested", "array" },
                      json::object { { "nested", "object" } } };

    // to_string测试
    std::string json_str = arr.to_string();
    if (json_str.empty()) {
        std::cerr << "to_string() test failed" << std::endl;
        return false;
    }

    // dumps测试（无缩进）
    std::string dumps_str = arr.dumps();
    if (dumps_str.empty()) {
        std::cerr << "dumps() test failed" << std::endl;
        return false;
    }

    // dumps测试（带缩进）
    std::string dumps_indent = arr.dumps(2);
    if (dumps_indent.empty()) {
        std::cerr << "dumps(indent) test failed" << std::endl;
        return false;
    }

    // format测试
    std::string formatted = arr.format(4);
    if (formatted.empty()) {
        std::cerr << "format() test failed" << std::endl;
        return false;
    }

    // 反序列化测试（通过parse）
    std::string test_json = R"([1, "hello", true, null, {"key": "value"}])";
    auto parsed = json::parse(test_json);
    if (!parsed.has_value() || !parsed->is_array()) {
        std::cerr << "Parse array test failed" << std::endl;
        return false;
    }

    json::array parsed_arr = parsed->as_array();
    if (parsed_arr.size() != 5 || parsed_arr[1].as_string() != "hello") {
        std::cerr << "Parsed array content test failed" << std::endl;
        return false;
    }

    // 转换为std::vector测试
    json::array int_arr { 1, 2, 3, 4, 5 };
    std::vector<int> vec = int_arr.as<std::vector<int>>();
    if (vec.size() != 5 || vec[2] != 3) {
        std::cerr << "as<vector>() test failed" << std::endl;
        return false;
    }

    // 转换为std::deque测试
    std::deque<int> deq = int_arr.as<std::deque<int>>();
    if (deq.size() != 5 || deq.front() != 1 || deq.back() != 5) {
        std::cerr << "as<deque>() test failed" << std::endl;
        return false;
    }

    // 嵌套容器测试
    json::array nested { json::array { 1, 2 }, json::array { 3, 4 }, json::array { 5, 6 } };
    std::vector<std::vector<int>> nested_vec = nested.as<std::vector<std::vector<int>>>();
    if (nested_vec.size() != 3 || nested_vec[1][1] != 4) {
        std::cerr << "Nested container conversion test failed" << std::endl;
        return false;
    }

    std::cout << "Array serialization test passed" << std::endl;
    return true;
}

