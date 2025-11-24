#include <iostream>
#include <limits>

#include "json.hpp"
#include "value_test.h"

bool test_value_constructors();
bool test_value_type_checks();
bool test_value_access_methods();
bool test_value_conversion_methods();
bool test_value_operators();
bool test_value_modification();
bool test_value_extended_conversions();

bool value_test()
{
    return test_value_constructors() && test_value_type_checks() && test_value_access_methods()
           && test_value_conversion_methods() && test_value_operators() && test_value_modification()
           && test_value_extended_conversions();
}

bool test_value_constructors()
{
    std::cout << "Testing value constructors..." << std::endl;

    // null构造
    json::value v_null;
    if (!v_null.is_null()) {
        std::cerr << "Default constructor should create null value" << std::endl;
        return false;
    }

    json::value v_nullptr = nullptr;
    if (!v_nullptr.is_null()) {
        std::cerr << "nullptr constructor should create null value" << std::endl;
        return false;
    }

    // boolean构造
    json::value v_true = true;
    json::value v_false = false;
    if (!v_true.is_boolean() || !v_false.is_boolean()) {
        std::cerr << "Boolean constructor failed" << std::endl;
        return false;
    }
    if (!v_true.as_boolean() || v_false.as_boolean()) {
        std::cerr << "Boolean value incorrect" << std::endl;
        return false;
    }

    // number构造（各种整数类型）
    json::value v_int = 42;
    json::value v_uint = 42u;
    json::value v_long = 42l;
    json::value v_ulong = 42ul;
    json::value v_llong = 42ll;
    json::value v_ullong = 42ull;
    if (!v_int.is_number() || !v_uint.is_number() || !v_long.is_number() || !v_ulong.is_number()
        || !v_llong.is_number() || !v_ullong.is_number()) {
        std::cerr << "Integer constructor failed" << std::endl;
        return false;
    }

    // number构造（浮点类型）
    json::value v_float = 3.14f;
    json::value v_double = 3.14159;
    json::value v_ldouble = 3.14159265358979323846l;
    if (!v_float.is_number() || !v_double.is_number() || !v_ldouble.is_number()) {
        std::cerr << "Float constructor failed" << std::endl;
        return false;
    }

    // string构造
    json::value v_cstr = "hello";
    json::value v_str = std::string("world");
    json::value v_strview = std::string_view("test");
    if (!v_cstr.is_string() || !v_str.is_string() || !v_strview.is_string()) {
        std::cerr << "String constructor failed" << std::endl;
        return false;
    }
    if (v_cstr.as_string() != "hello" || v_str.as_string() != "world" || v_strview.as_string() != "test") {
        std::cerr << "String value incorrect" << std::endl;
        return false;
    }

    // array构造
    json::value v_arr = json::array { 1, 2, 3 };
    if (!v_arr.is_array()) {
        std::cerr << "Array constructor failed" << std::endl;
        return false;
    }
    if (v_arr.as_array().size() != 3) {
        std::cerr << "Array size incorrect" << std::endl;
        return false;
    }

    // object构造
    json::value v_obj = json::object { { "key1", 1 }, { "key2", "value2" } };
    if (!v_obj.is_object()) {
        std::cerr << "Object constructor failed" << std::endl;
        return false;
    }
    if (v_obj.as_object().size() != 2) {
        std::cerr << "Object size incorrect" << std::endl;
        return false;
    }

    // 拷贝构造
    json::value v_copy = v_obj;
    if (!v_copy.is_object() || v_copy.as_object().size() != 2) {
        std::cerr << "Copy constructor failed" << std::endl;
        return false;
    }

    // 移动构造
    json::value v_move = std::move(v_copy);
    if (!v_move.is_object() || v_move.as_object().size() != 2) {
        std::cerr << "Move constructor failed" << std::endl;
        return false;
    }

    // enum构造
    enum class Color
    {
        Red = 1,
        Green = 2,
        Blue = 3
    };
    json::value v_enum = Color::Red;
#ifdef MEOJSON_ENUM_AS_NUMBER
    if (!v_enum.is_number() || v_enum.as_integer() != 1) {
        std::cerr << "Enum constructor failed" << std::endl;
        return false;
    }
#else
    if (!v_enum.is_string() || v_enum.as_string() != "Red") {
        std::cerr << "Enum constructor failed (expected string 'Red', got " << v_enum.to_string() << ")" << std::endl;
        return false;
    }
#endif

    std::cout << "Value constructors test passed" << std::endl;
    return true;
}

bool test_value_type_checks()
{
    std::cout << "Testing value type checks..." << std::endl;

    json::value v_null;
    json::value v_bool = true;
    json::value v_int = 42;
    json::value v_double = 3.14;
    json::value v_str = "hello";
    json::value v_arr = json::array { 1, 2, 3 };
    json::value v_obj = json::object { { "key", "value" } };

    // is_null测试
    if (!v_null.is_null() || v_bool.is_null() || v_int.is_null() || v_double.is_null() || v_str.is_null()
        || v_arr.is_null() || v_obj.is_null()) {
        std::cerr << "is_null() test failed" << std::endl;
        return false;
    }

    // is_boolean测试
    if (v_null.is_boolean() || !v_bool.is_boolean() || v_int.is_boolean() || v_double.is_boolean() || v_str.is_boolean()
        || v_arr.is_boolean() || v_obj.is_boolean()) {
        std::cerr << "is_boolean() test failed" << std::endl;
        return false;
    }

    // is_number测试
    if (v_null.is_number() || v_bool.is_number() || !v_int.is_number() || !v_double.is_number() || v_str.is_number()
        || v_arr.is_number() || v_obj.is_number()) {
        std::cerr << "is_number() test failed" << std::endl;
        return false;
    }

    // is_string测试
    if (v_null.is_string() || v_bool.is_string() || v_int.is_string() || v_double.is_string() || !v_str.is_string()
        || v_arr.is_string() || v_obj.is_string()) {
        std::cerr << "is_string() test failed" << std::endl;
        return false;
    }

    // is_array测试
    if (v_null.is_array() || v_bool.is_array() || v_int.is_array() || v_double.is_array() || v_str.is_array()
        || !v_arr.is_array() || v_obj.is_array()) {
        std::cerr << "is_array() test failed" << std::endl;
        return false;
    }

    // is_object测试
    if (v_null.is_object() || v_bool.is_object() || v_int.is_object() || v_double.is_object() || v_str.is_object()
        || v_arr.is_object() || !v_obj.is_object()) {
        std::cerr << "is_object() test failed" << std::endl;
        return false;
    }

    // valid测试
    if (!v_null.valid() || !v_bool.valid() || !v_int.valid() || !v_str.valid() || !v_arr.valid() || !v_obj.valid()) {
        std::cerr << "valid() test failed" << std::endl;
        return false;
    }

    // empty测试
    json::value v_empty_str = "";
    json::value v_empty_arr = json::array {};
    json::value v_empty_obj = json::object {};
    if (!v_empty_str.empty() || !v_empty_arr.empty() || !v_empty_obj.empty()) {
        std::cerr << "empty() test failed for empty containers" << std::endl;
        return false;
    }
    if (v_str.empty() || v_arr.empty() || v_obj.empty()) {
        std::cerr << "empty() test failed for non-empty containers" << std::endl;
        return false;
    }

    // type()测试
    if (v_null.type() != json::value::value_type::null) {
        std::cerr << "type() test failed for null" << std::endl;
        return false;
    }
    if (v_bool.type() != json::value::value_type::boolean) {
        std::cerr << "type() test failed for boolean" << std::endl;
        return false;
    }
    if (v_int.type() != json::value::value_type::number) {
        std::cerr << "type() test failed for number" << std::endl;
        return false;
    }
    if (v_str.type() != json::value::value_type::string) {
        std::cerr << "type() test failed for string" << std::endl;
        return false;
    }
    if (v_arr.type() != json::value::value_type::array) {
        std::cerr << "type() test failed for array" << std::endl;
        return false;
    }
    if (v_obj.type() != json::value::value_type::object) {
        std::cerr << "type() test failed for object" << std::endl;
        return false;
    }

    // is<T>()模板测试
    if (!v_bool.is<bool>()) {
        std::cerr << "is<bool>() test failed" << std::endl;
        return false;
    }
    if (!v_int.is<int>()) {
        std::cerr << "is<int>() test failed" << std::endl;
        return false;
    }
    if (!v_str.is<std::string>()) {
        std::cerr << "is<std::string>() test failed" << std::endl;
        return false;
    }

    // all<T>()测试
    json::value int_arr = json::array { 1, 2, 3, 4, 5 };
    if (!int_arr.all<int>()) {
        std::cerr << "all<int>() test failed for int array" << std::endl;
        return false;
    }
    json::value mixed_arr = json::array { 1, "two", 3 };
    if (mixed_arr.all<int>()) {
        std::cerr << "all<int>() test failed for mixed array" << std::endl;
        return false;
    }

    json::value int_obj = json::object { { "a", 1 }, { "b", 2 } };
    if (!int_obj.all<int>()) {
        std::cerr << "all<int>() test failed for int object" << std::endl;
        return false;
    }
    json::value mixed_obj = json::object { { "a", 1 }, { "b", "two" } };
    if (mixed_obj.all<int>()) {
        std::cerr << "all<int>() test failed for mixed object" << std::endl;
        return false;
    }

    std::cout << "Value type checks test passed" << std::endl;
    return true;
}

bool test_value_access_methods()
{
    std::cout << "Testing value access methods..." << std::endl;

    // 测试array访问
    json::value arr = json::array { 1, 2, 3, "four", 5 };

    // contains测试（通过索引）
    if (!arr.contains(0) || !arr.contains(4)) {
        std::cerr << "contains(size_t) test failed for valid indices" << std::endl;
        return false;
    }
    if (arr.contains(5)) {
        std::cerr << "contains(size_t) test failed for invalid index" << std::endl;
        return false;
    }

    // exists测试（通过索引）
    if (!arr.exists(0) || !arr.exists(4)) {
        std::cerr << "exists(size_t) test failed for valid indices" << std::endl;
        return false;
    }
    if (arr.exists(5)) {
        std::cerr << "exists(size_t) test failed for invalid index" << std::endl;
        return false;
    }

    // at测试（通过索引）
    if (arr.at(0).as_integer() != 1 || arr.at(3).as_string() != "four") {
        std::cerr << "at(size_t) test failed" << std::endl;
        return false;
    }

    // find测试（通过索引）
    auto found = arr.find<int>(0);
    if (!found.has_value() || found.value() != 1) {
        std::cerr << "find(size_t) test failed for existing element" << std::endl;
        return false;
    }
    auto not_found = arr.find(10);
    if (not_found.has_value()) {
        std::cerr << "find(size_t) test failed for non-existing element" << std::endl;
        return false;
    }

    // 测试object访问
    json::value obj = json::object { { "name", "Alice" }, { "age", 30 }, { "city", "NYC" } };

    // contains测试（通过键）
    if (!obj.contains("name") || !obj.contains("age")) {
        std::cerr << "contains(string) test failed for existing keys" << std::endl;
        return false;
    }
    if (obj.contains("country")) {
        std::cerr << "contains(string) test failed for non-existing key" << std::endl;
        return false;
    }

    // exists测试（通过键）
    if (!obj.exists("name") || !obj.exists("city")) {
        std::cerr << "exists(string) test failed for existing keys" << std::endl;
        return false;
    }
    if (obj.exists("country")) {
        std::cerr << "exists(string) test failed for non-existing key" << std::endl;
        return false;
    }

    // at测试（通过键）
    if (obj.at("name").as_string() != "Alice" || obj.at("age").as_integer() != 30) {
        std::cerr << "at(string) test failed" << std::endl;
        return false;
    }

    // find测试（通过键）
    auto found_name = obj.find<std::string>("name");
    if (!found_name.has_value() || found_name.value() != "Alice") {
        std::cerr << "find(string) test failed for existing key" << std::endl;
        return false;
    }
    auto not_found_key = obj.find("country");
    if (not_found_key.has_value()) {
        std::cerr << "find(string) test failed for non-existing key" << std::endl;
        return false;
    }

    // get测试（带默认值）
    int age = obj.get("age", 0);
    if (age != 30) {
        std::cerr << "get() test failed for existing key" << std::endl;
        return false;
    }
    int default_age = obj.get("unknown", 25);
    if (default_age != 25) {
        std::cerr << "get() test failed for non-existing key with default" << std::endl;
        return false;
    }

    // 嵌套get测试
    json::value nested = json::object { { "user", json::object { { "info", json::object { { "name", "Bob" } } } } } };
    std::string name = nested.get("user", "info", "name", std::string("Unknown"));
    if (name != "Bob") {
        std::cerr << "Nested get() test failed" << std::endl;
        return false;
    }
    std::string unknown = nested.get("user", "info", "age", std::string("N/A"));
    if (unknown != "N/A") {
        std::cerr << "Nested get() with default test failed" << std::endl;
        return false;
    }

    std::cout << "Value access methods test passed" << std::endl;
    return true;
}

bool test_value_conversion_methods()
{
    std::cout << "Testing value conversion methods..." << std::endl;

    // as_boolean测试
    json::value v_true = true;
    json::value v_false = false;
    if (!v_true.as_boolean() || v_false.as_boolean()) {
        std::cerr << "as_boolean() test failed" << std::endl;
        return false;
    }

    // as_integer系列测试
    json::value v_int = 42;
    if (v_int.as_integer() != 42) {
        std::cerr << "as_integer() test failed" << std::endl;
        return false;
    }
    if (v_int.as_long() != 42l) {
        std::cerr << "as_long() test failed" << std::endl;
        return false;
    }
    if (v_int.as_long_long() != 42ll) {
        std::cerr << "as_long_long() test failed" << std::endl;
        return false;
    }

    json::value v_uint = 42u;
    if (v_uint.as_unsigned() != 42u) {
        std::cerr << "as_unsigned() test failed" << std::endl;
        return false;
    }
    if (v_uint.as_unsigned_long() != 42ul) {
        std::cerr << "as_unsigned_long() test failed" << std::endl;
        return false;
    }
    if (v_uint.as_unsigned_long_long() != 42ull) {
        std::cerr << "as_unsigned_long_long() test failed" << std::endl;
        return false;
    }

    // 浮点数测试
    json::value v_float = 3.14f;
    if (std::abs(v_float.as_float() - 3.14f) > 0.001f) {
        std::cerr << "as_float() test failed" << std::endl;
        return false;
    }

    json::value v_double = 3.14159;
    if (std::abs(v_double.as_double() - 3.14159) > 0.00001) {
        std::cerr << "as_double() test failed" << std::endl;
        return false;
    }

    // as_string测试
    json::value v_str = "hello world";
    if (v_str.as_string() != "hello world") {
        std::cerr << "as_string() test failed" << std::endl;
        return false;
    }

    // as_array测试
    json::value v_arr = json::array { 1, 2, 3 };
    const json::array& arr = v_arr.as_array();
    if (arr.size() != 3) {
        std::cerr << "as_array() test failed" << std::endl;
        return false;
    }

    // as_object测试
    json::value v_obj = json::object { { "key", "value" } };
    const json::object& obj = v_obj.as_object();
    if (obj.size() != 1) {
        std::cerr << "as_object() test failed" << std::endl;
        return false;
    }

    // explicit operator测试
    json::value v_bool = true;
    bool b = (bool)v_bool;
    if (!b) {
        std::cerr << "operator bool() test failed" << std::endl;
        return false;
    }

    json::value v_int2 = 100;
    int i = (int)v_int2;
    if (i != 100) {
        std::cerr << "operator int() test failed" << std::endl;
        return false;
    }

    json::value v_str2 = "test";
    std::string s = (std::string)v_str2;
    if (s != "test") {
        std::cerr << "operator string() test failed" << std::endl;
        return false;
    }

    // as<T>模板测试
    json::value v_vec = json::array { 1, 2, 3, 4, 5 };
    std::vector<int> vec = v_vec.as<std::vector<int>>();
    if (vec.size() != 5 || vec[0] != 1 || vec[4] != 5) {
        std::cerr << "as<vector<int>>() test failed" << std::endl;
        return false;
    }

    json::value v_map = json::object { { "a", 1 }, { "b", 2 } };
    std::map<std::string, int> map = v_map.as<std::map<std::string, int>>();
    if (map.size() != 2 || map["a"] != 1 || map["b"] != 2) {
        std::cerr << "as<map<string, int>>() test failed" << std::endl;
        return false;
    }

    std::cout << "Value conversion methods test passed" << std::endl;
    return true;
}

bool test_value_operators()
{
    std::cout << "Testing value operators..." << std::endl;

    // operator[]测试（array）
    json::value arr = json::array { 1, 2, 3 };
    if (arr[0].as_integer() != 1 || arr[2].as_integer() != 3) {
        std::cerr << "operator[] for array test failed" << std::endl;
        return false;
    }
    arr[1] = 20;
    if (arr[1].as_integer() != 20) {
        std::cerr << "operator[] assignment for array test failed" << std::endl;
        return false;
    }

    // operator[]测试（object）
    json::value obj = json::object { { "a", 1 }, { "b", 2 } };
    if (obj["a"].as_integer() != 1 || obj["b"].as_integer() != 2) {
        std::cerr << "operator[] for object test failed" << std::endl;
        return false;
    }
    obj["c"] = 3;
    if (obj["c"].as_integer() != 3) {
        std::cerr << "operator[] insertion for object test failed" << std::endl;
        return false;
    }

    // operator==和operator!=测试
    json::value v1 = 42;
    json::value v2 = 42;
    json::value v3 = 43;
    if (!(v1 == v2) || v1 != v2) {
        std::cerr << "operator== test failed for equal values" << std::endl;
        return false;
    }
    if (v1 == v3 || !(v1 != v3)) {
        std::cerr << "operator!= test failed for different values" << std::endl;
        return false;
    }

    // object union操作符测试
    json::value obj1 = json::object { { "a", 1 }, { "b", 2 } };
    json::value obj2 = json::object { { "b", 3 }, { "c", 4 } };
    json::value obj3 = obj1 | obj2.as_object();
    if (obj3["a"].as_integer() != 1 || obj3["b"].as_integer() != 3 || obj3["c"].as_integer() != 4) {
        std::cerr << "operator| for object test failed" << std::endl;
        return false;
    }

    // object |=操作符测试
    json::value obj4 = json::object { { "x", 1 } };
    obj4 |= json::object { { "y", 2 } };
    if (obj4["x"].as_integer() != 1 || obj4["y"].as_integer() != 2) {
        std::cerr << "operator|= for object test failed" << std::endl;
        return false;
    }

    // array拼接操作符测试
    json::value arr1 = json::array { 1, 2 };
    json::value arr2 = json::array { 3, 4 };
    json::value arr3 = arr1 + arr2.as_array();
    if (arr3.as_array().size() != 4 || arr3[0].as_integer() != 1 || arr3[3].as_integer() != 4) {
        std::cerr << "operator+ for array test failed" << std::endl;
        return false;
    }

    // array +=操作符测试
    json::value arr4 = json::array { 1, 2 };
    arr4 += json::array { 3, 4 };
    if (arr4.as_array().size() != 4 || arr4[2].as_integer() != 3) {
        std::cerr << "operator+= for array test failed" << std::endl;
        return false;
    }

    // 赋值操作符测试
    json::value v_src = 123;
    json::value v_dst;
    v_dst = v_src;
    if (v_dst.as_integer() != 123) {
        std::cerr << "operator= test failed" << std::endl;
        return false;
    }

    std::cout << "Value operators test passed" << std::endl;
    return true;
}

bool test_value_modification()
{
    std::cout << "Testing value modification..." << std::endl;

    // emplace测试（array）
    json::value arr = json::array {};
    arr.emplace(1);
    arr.emplace(2);
    arr.emplace("three");
    if (arr.as_array().size() != 3 || arr[2].as_string() != "three") {
        std::cerr << "emplace() for array test failed" << std::endl;
        return false;
    }

    // emplace测试（object）
    json::value obj = json::object {};
    obj.emplace("key1", "value1");
    obj.emplace("key2", 42);
    if (obj.as_object().size() != 2 || obj["key2"].as_integer() != 42) {
        std::cerr << "emplace() for object test failed" << std::endl;
        return false;
    }

    // erase测试（array）
    json::value arr2 = json::array { 1, 2, 3, 4, 5 };
    bool erased = arr2.erase(2);
    if (!erased || arr2.as_array().size() != 4 || arr2[2].as_integer() != 4) {
        std::cerr << "erase(size_t) test failed" << std::endl;
        return false;
    }
    bool not_erased = arr2.erase(10);
    if (not_erased) {
        std::cerr << "erase(size_t) should return false for invalid index" << std::endl;
        return false;
    }

    // erase测试（object）
    json::value obj2 = json::object { { "a", 1 }, { "b", 2 }, { "c", 3 } };
    bool erased_key = obj2.erase("b");
    if (!erased_key || obj2.as_object().size() != 2 || obj2.contains("b")) {
        std::cerr << "erase(string) test failed" << std::endl;
        return false;
    }
    bool not_erased_key = obj2.erase("d");
    if (not_erased_key) {
        std::cerr << "erase(string) should return false for non-existing key" << std::endl;
        return false;
    }

    // clear测试
    json::value arr3 = json::array { 1, 2, 3 };
    arr3.clear();
    if (!arr3.empty()) {
        std::cerr << "clear() for array test failed" << std::endl;
        return false;
    }

    json::value obj3 = json::object { { "a", 1 } };
    obj3.clear();
    if (!obj3.empty()) {
        std::cerr << "clear() for object test failed" << std::endl;
        return false;
    }

    // 序列化测试
    json::value test_val = json::object {
        { "string", "hello" },
        { "number", 42 },
        { "float", 3.14 },
        { "bool", true },
        { "null", nullptr },
        { "array", json::array { 1, 2, 3 } },
        { "object", json::object { { "nested", "value" } } }
    };

    std::string json_str = test_val.to_string();
    if (json_str.empty()) {
        std::cerr << "to_string() test failed" << std::endl;
        return false;
    }

    std::string formatted = test_val.format(2);
    if (formatted.empty()) {
        std::cerr << "format() test failed" << std::endl;
        return false;
    }

    std::string dumps = test_val.dumps();
    if (dumps.empty()) {
        std::cerr << "dumps() test failed" << std::endl;
        return false;
    }

    std::string dumps_indent = test_val.dumps(2);
    if (dumps_indent.empty()) {
        std::cerr << "dumps(indent) test failed" << std::endl;
        return false;
    }

    std::cout << "Value modification test passed" << std::endl;
    return true;
}

bool test_value_extended_conversions()
{
    std::cout << "Testing value extended conversions..." << std::endl;

    // 测试 std::monostate 构造函数
    json::value v_monostate = std::monostate {};
    if (!v_monostate.is_null()) {
        std::cerr << "std::monostate constructor should create null value" << std::endl;
        return false;
    }

    // 测试 is<std::monostate>()
    json::value v_null;
    if (!v_null.is<std::monostate>()) {
        std::cerr << "is<std::monostate>() test failed for null value" << std::endl;
        return false;
    }

    json::value v_int = 42;
    if (v_int.is<std::monostate>()) {
        std::cerr << "is<std::monostate>() should return false for non-null value" << std::endl;
        return false;
    }

    // 测试 is<std::nullptr_t>()
    if (!v_null.is<std::nullptr_t>()) {
        std::cerr << "is<std::nullptr_t>() test failed for null value" << std::endl;
        return false;
    }

    if (v_int.is<std::nullptr_t>()) {
        std::cerr << "is<std::nullptr_t>() should return false for non-null value" << std::endl;
        return false;
    }

    // 测试 as<std::monostate>()
    try {
        std::monostate mono = v_null.as<std::monostate>();
        // 成功转换，符合预期
    }
    catch (const std::exception&) {
        std::cerr << "as<std::monostate>() test failed for null value" << std::endl;
        return false;
    }

    // 测试 as<std::monostate>() 对非null值应抛出异常
    try {
        std::monostate mono = v_int.as<std::monostate>();
        std::cerr << "as<std::monostate>() should throw for non-null value" << std::endl;
        return false;
    }
    catch (const json::exception&) {
        // 预期抛出异常
    }

    // 测试 operator std::monostate()
    try {
        std::monostate mono = (std::monostate)v_null;
        // 成功转换，符合预期
    }
    catch (const std::exception&) {
        std::cerr << "operator std::monostate() test failed for null value" << std::endl;
        return false;
    }

    // 测试 operator std::monostate() 对非null值应抛出异常
    try {
        std::monostate mono = (std::monostate)v_int;
        std::cerr << "operator std::monostate() should throw for non-null value" << std::endl;
        return false;
    }
    catch (const json::exception&) {
        // 预期抛出异常
    }

    // 测试 operator std::nullptr_t()
    try {
        std::nullptr_t null_ptr = (std::nullptr_t)v_null;
        if (null_ptr != nullptr) {
            std::cerr << "operator std::nullptr_t() should return nullptr" << std::endl;
            return false;
        }
    }
    catch (const std::exception&) {
        std::cerr << "operator std::nullptr_t() test failed for null value" << std::endl;
        return false;
    }

    // 测试 operator std::nullptr_t() 对非null值应抛出异常
    try {
        std::nullptr_t null_ptr = (std::nullptr_t)v_int;
        std::cerr << "operator std::nullptr_t() should throw for non-null value" << std::endl;
        return false;
    }
    catch (const json::exception&) {
        // 预期抛出异常
    }

    // 测试 as_string_view()
    json::value v_str = "hello world";
    try {
        std::string_view sv = v_str.as_string_view();
        if (sv != "hello world") {
            std::cerr << "as_string_view() returned incorrect value" << std::endl;
            return false;
        }
    }
    catch (const std::exception&) {
        std::cerr << "as_string_view() test failed for string value" << std::endl;
        return false;
    }

    // 测试 as_string_view() 对非字符串值应抛出异常
    try {
        std::string_view sv = v_int.as_string_view();
        std::cerr << "as_string_view() should throw for non-string value" << std::endl;
        return false;
    }
    catch (const json::exception&) {
        // 预期抛出异常
    }

    // 测试 operator std::string_view()
    try {
        std::string_view sv = (std::string_view)v_str;
        if (sv != "hello world") {
            std::cerr << "operator std::string_view() returned incorrect value" << std::endl;
            return false;
        }
    }
    catch (const std::exception&) {
        std::cerr << "operator std::string_view() test failed for string value" << std::endl;
        return false;
    }

    // 测试 operator std::string_view() 对非字符串值应抛出异常
    try {
        std::string_view sv = (std::string_view)v_int;
        std::cerr << "operator std::string_view() should throw for non-string value" << std::endl;
        return false;
    }
    catch (const json::exception&) {
        // 预期抛出异常
    }

    // 测试 std::string_view 的生命周期安全性（左值引用）
    json::value v_temp_str = "temporary string";
    std::string_view sv_ref = (std::string_view)v_temp_str;
    if (sv_ref != "temporary string") {
        std::cerr << "string_view reference test failed" << std::endl;
        return false;
    }

#ifdef MEOJSON_FS_PATH_EXTENSION
    // 测试 std::filesystem::path 转换（左值）
    json::value v_path_str = "/usr/local/bin";
    try {
        std::filesystem::path p = (std::filesystem::path)v_path_str;
        if (p.string() != "/usr/local/bin") {
            std::cerr << "operator std::filesystem::path() (lvalue) returned incorrect value" << std::endl;
            return false;
        }
    }
    catch (const std::exception&) {
        std::cerr << "operator std::filesystem::path() (lvalue) test failed" << std::endl;
        return false;
    }

    // 测试 std::filesystem::path 转换（右值）
    try {
        std::filesystem::path p = (std::filesystem::path)json::value("/tmp/test");
        if (p.string() != "/tmp/test") {
            std::cerr << "operator std::filesystem::path() (rvalue) returned incorrect value" << std::endl;
            return false;
        }
    }
    catch (const std::exception&) {
        std::cerr << "operator std::filesystem::path() (rvalue) test failed" << std::endl;
        return false;
    }
#endif

    // 测试 std::variant 支持 std::monostate
    using variant_with_monostate = std::variant<std::monostate, int, std::string>;
    
    // 从 variant 构造 value
    json::value v_from_variant1 = variant_with_monostate { std::monostate {} };
    if (!v_from_variant1.is_null()) {
        std::cerr << "value constructor from variant<monostate> should create null value" << std::endl;
        return false;
    }

    json::value v_from_variant2 = variant_with_monostate { 42 };
    if (!v_from_variant2.is_number() || v_from_variant2.as_integer() != 42) {
        std::cerr << "value constructor from variant<int> failed" << std::endl;
        return false;
    }

    json::value v_from_variant3 = variant_with_monostate { std::string("test") };
    if (!v_from_variant3.is_string() || v_from_variant3.as_string() != "test") {
        std::cerr << "value constructor from variant<string> failed" << std::endl;
        return false;
    }

    std::cout << "Value extended conversions test passed" << std::endl;
    return true;
}

