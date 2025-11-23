#include <iostream>
#include <map>
#include <unordered_map>

#include "json.hpp"
#include "object_test.h"

bool test_object_constructors();
bool test_object_access_methods();
bool test_object_modification();
bool test_object_iterators();
bool test_object_operators();
bool test_object_serialization();

bool object_test()
{
    return test_object_constructors() && test_object_access_methods() && test_object_modification()
           && test_object_iterators() && test_object_operators() && test_object_serialization();
}

bool test_object_constructors()
{
    std::cout << "Testing object constructors..." << std::endl;

    // 默认构造
    json::object obj1;
    if (!obj1.empty() || obj1.size() != 0) {
        std::cerr << "Default constructor test failed" << std::endl;
        return false;
    }

    // 初始化列表构造
    json::object obj2 { { "key1", 1 }, { "key2", "value2" }, { "key3", true } };
    if (obj2.size() != 3) {
        std::cerr << "Initializer list constructor test failed" << std::endl;
        return false;
    }

    // 拷贝构造
    json::object obj3 = obj2;
    if (obj3.size() != 3) {
        std::cerr << "Copy constructor test failed" << std::endl;
        return false;
    }

    // 移动构造
    json::object obj4 = std::move(obj3);
    if (obj4.size() != 3) {
        std::cerr << "Move constructor test failed" << std::endl;
        return false;
    }

    // 从std::map构造
    std::map<std::string, int> map1 { { "a", 1 }, { "b", 2 }, { "c", 3 } };
    json::object obj5 = map1;
    if (obj5.size() != 3 || obj5["b"].as_integer() != 2) {
        std::cerr << "Constructor from std::map test failed" << std::endl;
        return false;
    }

    // 从std::unordered_map构造
    std::unordered_map<std::string, std::string> umap { { "x", "hello" }, { "y", "world" } };
    json::object obj6 = umap;
    if (obj6.size() != 2) {
        std::cerr << "Constructor from std::unordered_map test failed" << std::endl;
        return false;
    }

    std::cout << "Object constructors test passed" << std::endl;
    return true;
}

bool test_object_access_methods()
{
    std::cout << "Testing object access methods..." << std::endl;

    json::object obj { { "name", "Alice" }, { "age", 30 }, { "city", "NYC" }, { "active", true } };

    // empty和size测试
    if (obj.empty() || obj.size() != 4) {
        std::cerr << "empty() or size() test failed" << std::endl;
        return false;
    }

    // contains测试
    if (!obj.contains("name") || !obj.contains("age") || obj.contains("country")) {
        std::cerr << "contains() test failed" << std::endl;
        return false;
    }

    // exists测试
    if (!obj.exists("city") || obj.exists("unknown")) {
        std::cerr << "exists() test failed" << std::endl;
        return false;
    }

    // at测试
    if (obj.at("name").as_string() != "Alice") {
        std::cerr << "at() test failed" << std::endl;
        return false;
    }

    // operator[]测试
    if (obj["age"].as_integer() != 30) {
        std::cerr << "operator[] test failed" << std::endl;
        return false;
    }

    // get测试（带默认值）
    std::string name = obj.get("name", std::string("Unknown"));
    if (name != "Alice") {
        std::cerr << "get() test failed for existing key" << std::endl;
        return false;
    }

    int default_salary = obj.get("salary", 50000);
    if (default_salary != 50000) {
        std::cerr << "get() test failed for non-existing key with default" << std::endl;
        return false;
    }

    // find测试
    auto found = obj.find<std::string>("city");
    if (!found.has_value() || found.value() != "NYC") {
        std::cerr << "find() test failed for existing key" << std::endl;
        return false;
    }

    auto not_found = obj.find("unknown");
    if (not_found.has_value()) {
        std::cerr << "find() test failed for non-existing key" << std::endl;
        return false;
    }

    // all<T>测试
    json::object int_obj { { "a", 1 }, { "b", 2 }, { "c", 3 } };
    if (!int_obj.all<int>()) {
        std::cerr << "all<int>() test failed for int object" << std::endl;
        return false;
    }

    json::object mixed_obj { { "a", 1 }, { "b", "two" } };
    if (mixed_obj.all<int>()) {
        std::cerr << "all<int>() test failed for mixed object" << std::endl;
        return false;
    }

    std::cout << "Object access methods test passed" << std::endl;
    return true;
}

bool test_object_modification()
{
    std::cout << "Testing object modification..." << std::endl;

    json::object obj;

    // emplace测试
    obj.emplace("key1", "value1");
    obj.emplace("key2", 42);
    obj.emplace("key3", true);
    if (obj.size() != 3 || obj["key2"].as_integer() != 42) {
        std::cerr << "emplace() test failed" << std::endl;
        return false;
    }

    // insert测试
    obj.insert("key4", 3.14);
    if (obj.size() != 4) {
        std::cerr << "insert() test failed" << std::endl;
        return false;
    }

    // operator[]插入测试
    obj["key5"] = "new value";
    if (obj.size() != 5 || obj["key5"].as_string() != "new value") {
        std::cerr << "operator[] insertion test failed" << std::endl;
        return false;
    }

    // erase测试（通过键）
    bool erased = obj.erase("key3");
    if (!erased || obj.size() != 4 || obj.contains("key3")) {
        std::cerr << "erase(key) test failed" << std::endl;
        return false;
    }

    bool not_erased = obj.erase("nonexistent");
    if (not_erased) {
        std::cerr << "erase() should return false for non-existing key" << std::endl;
        return false;
    }

    // erase测试（通过迭代器）
    auto it = obj.begin();
    bool erased_by_iter = obj.erase(it);
    if (!erased_by_iter || obj.size() != 3) {
        std::cerr << "erase(iterator) test failed" << std::endl;
        return false;
    }

    // clear测试
    obj.clear();
    if (!obj.empty() || obj.size() != 0) {
        std::cerr << "clear() test failed" << std::endl;
        return false;
    }

    std::cout << "Object modification test passed" << std::endl;
    return true;
}

bool test_object_iterators()
{
    std::cout << "Testing object iterators..." << std::endl;

    json::object obj { { "a", 1 }, { "b", 2 }, { "c", 3 } };

    // begin/end测试
    int count = 0;
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        count++;
    }
    if (count != 3) {
        std::cerr << "begin()/end() iterator test failed" << std::endl;
        return false;
    }

    // cbegin/cend测试
    count = 0;
    for (auto it = obj.cbegin(); it != obj.cend(); ++it) {
        count++;
    }
    if (count != 3) {
        std::cerr << "cbegin()/cend() iterator test failed" << std::endl;
        return false;
    }

    // 范围for循环测试
    count = 0;
    int sum = 0;
    for (const auto& [key, value] : obj) {
        count++;
        sum += value.as_integer();
    }
    if (count != 3 || sum != 6) {
        std::cerr << "Range-based for loop test failed" << std::endl;
        return false;
    }

    // 修改测试
    for (auto& [key, value] : obj) {
        if (value.is_number()) {
            value = value.as_integer() * 2;
        }
    }
    if (obj["a"].as_integer() != 2 || obj["b"].as_integer() != 4 || obj["c"].as_integer() != 6) {
        std::cerr << "Iterator modification test failed" << std::endl;
        return false;
    }

    std::cout << "Object iterators test passed" << std::endl;
    return true;
}

bool test_object_operators()
{
    std::cout << "Testing object operators..." << std::endl;

    // operator==测试
    json::object obj1 { { "a", 1 }, { "b", 2 } };
    json::object obj2 { { "a", 1 }, { "b", 2 } };
    json::object obj3 { { "a", 1 }, { "b", 3 } };

    if (!(obj1 == obj2)) {
        std::cerr << "operator== test failed for equal objects" << std::endl;
        return false;
    }
    if (obj1 == obj3) {
        std::cerr << "operator== test failed for different objects" << std::endl;
        return false;
    }

    // operator!=测试
    if (obj1 != obj2) {
        std::cerr << "operator!= test failed for equal objects" << std::endl;
        return false;
    }
    if (!(obj1 != obj3)) {
        std::cerr << "operator!= test failed for different objects" << std::endl;
        return false;
    }

    // operator|测试（union操作）
    json::object obj4 { { "a", 1 }, { "b", 2 } };
    json::object obj5 { { "b", 3 }, { "c", 4 } };
    json::object obj6 = obj4 | obj5;

    if (obj6.size() != 3 || obj6["a"].as_integer() != 1 || obj6["b"].as_integer() != 3 || obj6["c"].as_integer() != 4) {
        std::cerr << "operator| test failed" << std::endl;
        return false;
    }

    // operator|=测试
    json::object obj7 { { "x", 10 } };
    obj7 |= json::object { { "y", 20 }, { "z", 30 } };

    if (obj7.size() != 3 || obj7["x"].as_integer() != 10 || obj7["y"].as_integer() != 20 || obj7["z"].as_integer() != 30) {
        std::cerr << "operator|= test failed" << std::endl;
        return false;
    }

    // operator=测试
    json::object obj8;
    obj8 = obj1;
    if (obj8.size() != 2 || obj8["a"].as_integer() != 1) {
        std::cerr << "operator= test failed" << std::endl;
        return false;
    }

    std::cout << "Object operators test passed" << std::endl;
    return true;
}

bool test_object_serialization()
{
    std::cout << "Testing object serialization..." << std::endl;

    json::object obj { { "name", "Bob" },
                       { "age", 25 },
                       { "scores", json::array { 85, 90, 95 } },
                       { "address", json::object { { "city", "LA" }, { "zip", "90001" } } } };

    // to_string测试
    std::string json_str = obj.to_string();
    if (json_str.empty()) {
        std::cerr << "to_string() test failed" << std::endl;
        return false;
    }

    // dumps测试（无缩进）
    std::string dumps_str = obj.dumps();
    if (dumps_str.empty()) {
        std::cerr << "dumps() test failed" << std::endl;
        return false;
    }

    // dumps测试（带缩进）
    std::string dumps_indent = obj.dumps(2);
    if (dumps_indent.empty()) {
        std::cerr << "dumps(indent) test failed" << std::endl;
        return false;
    }

    // format测试
    std::string formatted = obj.format(4);
    if (formatted.empty()) {
        std::cerr << "format() test failed" << std::endl;
        return false;
    }

    // 反序列化测试（通过parse）
    std::string test_json = R"({"key1": "value1", "key2": 42, "key3": true})";
    auto parsed = json::parse(test_json);
    if (!parsed.has_value() || !parsed->is_object()) {
        std::cerr << "Parse object test failed" << std::endl;
        return false;
    }

    json::object parsed_obj = parsed->as_object();
    if (parsed_obj.size() != 3 || parsed_obj["key2"].as_integer() != 42) {
        std::cerr << "Parsed object content test failed" << std::endl;
        return false;
    }

    // 转换为std::map测试
    json::object simple_obj { { "a", 1 }, { "b", 2 }, { "c", 3 } };
    std::map<std::string, int> result_map = simple_obj.as<std::map<std::string, int>>();
    if (result_map.size() != 3 || result_map["b"] != 2) {
        std::cerr << "as<map>() test failed" << std::endl;
        return false;
    }

    std::cout << "Object serialization test passed" << std::endl;
    return true;
}

