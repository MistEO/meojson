#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "json.hpp"
#include "smart_ptr_test.h"

bool test_shared_ptr_constructors();
bool test_unique_ptr_constructors();
bool test_smart_ptr_is_checks();
bool test_smart_ptr_conversions();
bool test_smart_ptr_as_methods();
bool test_smart_ptr_complex_types();

bool smart_ptr_test()
{
    return test_shared_ptr_constructors() && test_unique_ptr_constructors() && test_smart_ptr_is_checks()
           && test_smart_ptr_conversions() && test_smart_ptr_as_methods() && test_smart_ptr_complex_types();
}

bool test_shared_ptr_constructors()
{
    std::cout << "Testing shared_ptr constructors..." << std::endl;

    // 测试 shared_ptr<int> 构造
    std::shared_ptr<int> ptr_int = std::make_shared<int>(42);
    json::value v_int = ptr_int;
    if (!v_int.is_number() || v_int.as_integer() != 42) {
        std::cerr << "shared_ptr<int> constructor failed" << std::endl;
        return false;
    }

    // 测试 null shared_ptr 构造
    std::shared_ptr<int> ptr_null = nullptr;
    json::value v_null = ptr_null;
    if (!v_null.is_null()) {
        std::cerr << "null shared_ptr constructor should create null value" << std::endl;
        return false;
    }

    // 测试 shared_ptr<std::string> 构造
    std::shared_ptr<std::string> ptr_str = std::make_shared<std::string>("hello");
    json::value v_str = ptr_str;
    if (!v_str.is_string() || v_str.as_string() != "hello") {
        std::cerr << "shared_ptr<string> constructor failed" << std::endl;
        return false;
    }

    // 测试 shared_ptr<bool> 构造
    std::shared_ptr<bool> ptr_bool = std::make_shared<bool>(true);
    json::value v_bool = ptr_bool;
    if (!v_bool.is_boolean() || !v_bool.as_boolean()) {
        std::cerr << "shared_ptr<bool> constructor failed" << std::endl;
        return false;
    }

    // 测试 shared_ptr<double> 构造
    std::shared_ptr<double> ptr_double = std::make_shared<double>(3.14);
    json::value v_double = ptr_double;
    if (!v_double.is_number()) {
        std::cerr << "shared_ptr<double> constructor failed" << std::endl;
        return false;
    }

    std::cout << "shared_ptr constructors test passed" << std::endl;
    return true;
}

bool test_unique_ptr_constructors()
{
    std::cout << "Testing unique_ptr constructors..." << std::endl;

    // 测试 unique_ptr<int> 构造
    std::unique_ptr<int> ptr_int = std::make_unique<int>(42);
    json::value v_int = std::move(ptr_int);
    if (!v_int.is_number() || v_int.as_integer() != 42) {
        std::cerr << "unique_ptr<int> constructor failed" << std::endl;
        return false;
    }

    // 测试 null unique_ptr 构造
    std::unique_ptr<int> ptr_null = nullptr;
    json::value v_null = std::move(ptr_null);
    if (!v_null.is_null()) {
        std::cerr << "null unique_ptr constructor should create null value" << std::endl;
        return false;
    }

    // 测试 unique_ptr<std::string> 构造
    std::unique_ptr<std::string> ptr_str = std::make_unique<std::string>("world");
    json::value v_str = std::move(ptr_str);
    if (!v_str.is_string() || v_str.as_string() != "world") {
        std::cerr << "unique_ptr<string> constructor failed" << std::endl;
        return false;
    }

    // 测试 unique_ptr<bool> 构造
    std::unique_ptr<bool> ptr_bool = std::make_unique<bool>(false);
    json::value v_bool = std::move(ptr_bool);
    if (!v_bool.is_boolean() || v_bool.as_boolean()) {
        std::cerr << "unique_ptr<bool> constructor failed" << std::endl;
        return false;
    }

    // 测试 unique_ptr<double> 构造
    std::unique_ptr<double> ptr_double = std::make_unique<double>(2.71);
    json::value v_double = std::move(ptr_double);
    if (!v_double.is_number()) {
        std::cerr << "unique_ptr<double> constructor failed" << std::endl;
        return false;
    }

    std::cout << "unique_ptr constructors test passed" << std::endl;
    return true;
}

bool test_smart_ptr_is_checks()
{
    std::cout << "Testing smart pointer is<> checks..." << std::endl;

    // 测试 is<shared_ptr<int>>
    json::value v_int = 42;
    if (!v_int.is<std::shared_ptr<int>>()) {
        std::cerr << "is<shared_ptr<int>>() test failed for int value" << std::endl;
        return false;
    }

    json::value v_null;
    if (!v_null.is<std::shared_ptr<int>>()) {
        std::cerr << "is<shared_ptr<int>>() test failed for null value" << std::endl;
        return false;
    }

    json::value v_str = "hello";
    if (v_str.is<std::shared_ptr<int>>()) {
        std::cerr << "is<shared_ptr<int>>() should return false for string value" << std::endl;
        return false;
    }

    // 测试 is<unique_ptr<int>>
    if (!v_int.is<std::unique_ptr<int>>()) {
        std::cerr << "is<unique_ptr<int>>() test failed for int value" << std::endl;
        return false;
    }

    if (!v_null.is<std::unique_ptr<int>>()) {
        std::cerr << "is<unique_ptr<int>>() test failed for null value" << std::endl;
        return false;
    }

    if (v_str.is<std::unique_ptr<int>>()) {
        std::cerr << "is<unique_ptr<int>>() should return false for string value" << std::endl;
        return false;
    }

    // 测试 is<shared_ptr<string>>
    if (!v_str.is<std::shared_ptr<std::string>>()) {
        std::cerr << "is<shared_ptr<string>>() test failed for string value" << std::endl;
        return false;
    }

    if (v_int.is<std::shared_ptr<std::string>>()) {
        std::cerr << "is<shared_ptr<string>>() should return false for int value" << std::endl;
        return false;
    }

    // 测试 is<unique_ptr<string>>
    if (!v_str.is<std::unique_ptr<std::string>>()) {
        std::cerr << "is<unique_ptr<string>>() test failed for string value" << std::endl;
        return false;
    }

    if (v_int.is<std::unique_ptr<std::string>>()) {
        std::cerr << "is<unique_ptr<string>>() should return false for int value" << std::endl;
        return false;
    }

    std::cout << "smart pointer is<> checks test passed" << std::endl;
    return true;
}

bool test_smart_ptr_conversions()
{
    std::cout << "Testing smart pointer conversions..." << std::endl;

    // 测试 operator shared_ptr<int>() 左值
    json::value v_int = 100;
    std::shared_ptr<int> ptr_int = static_cast<std::shared_ptr<int>>(v_int);
    if (ptr_int == nullptr || *ptr_int != 100) {
        std::cerr << "operator shared_ptr<int>() (lvalue) test failed" << std::endl;
        return false;
    }

    // 测试 operator shared_ptr<int>() 右值
    std::shared_ptr<int> ptr_int2 = static_cast<std::shared_ptr<int>>(json::value(200));
    if (ptr_int2 == nullptr || *ptr_int2 != 200) {
        std::cerr << "operator shared_ptr<int>() (rvalue) test failed" << std::endl;
        return false;
    }

    // 测试 null 转换为 shared_ptr
    json::value v_null;
    std::shared_ptr<int> ptr_null = static_cast<std::shared_ptr<int>>(v_null);
    if (ptr_null != nullptr) {
        std::cerr << "null value should convert to nullptr shared_ptr" << std::endl;
        return false;
    }

    // 测试 operator unique_ptr<int>() 左值
    json::value v_int3 = 300;
    std::unique_ptr<int> uptr_int = static_cast<std::unique_ptr<int>>(v_int3);
    if (uptr_int == nullptr || *uptr_int != 300) {
        std::cerr << "operator unique_ptr<int>() (lvalue) test failed" << std::endl;
        return false;
    }

    // 测试 operator unique_ptr<int>() 右值
    std::unique_ptr<int> uptr_int2 = static_cast<std::unique_ptr<int>>(json::value(400));
    if (uptr_int2 == nullptr || *uptr_int2 != 400) {
        std::cerr << "operator unique_ptr<int>() (rvalue) test failed" << std::endl;
        return false;
    }

    // 测试 null 转换为 unique_ptr
    std::unique_ptr<int> uptr_null = static_cast<std::unique_ptr<int>>(v_null);
    if (uptr_null != nullptr) {
        std::cerr << "null value should convert to nullptr unique_ptr" << std::endl;
        return false;
    }

    // 测试 shared_ptr<string> 转换
    json::value v_str = "test";
    std::shared_ptr<std::string> ptr_str = static_cast<std::shared_ptr<std::string>>(v_str);
    if (ptr_str == nullptr || *ptr_str != "test") {
        std::cerr << "operator shared_ptr<string>() test failed" << std::endl;
        return false;
    }

    // 测试 unique_ptr<string> 转换
    json::value v_str2 = "test2";
    std::unique_ptr<std::string> uptr_str = static_cast<std::unique_ptr<std::string>>(v_str2);
    if (uptr_str == nullptr || *uptr_str != "test2") {
        std::cerr << "operator unique_ptr<string>() test failed" << std::endl;
        return false;
    }

    std::cout << "smart pointer conversions test passed" << std::endl;
    return true;
}

bool test_smart_ptr_as_methods()
{
    std::cout << "Testing smart pointer as<> methods..." << std::endl;

    // 测试 as<shared_ptr<int>>() 左值
    json::value v_int = 42;
    std::shared_ptr<int> ptr_int = v_int.as<std::shared_ptr<int>>();
    if (ptr_int == nullptr || *ptr_int != 42) {
        std::cerr << "as<shared_ptr<int>>() (lvalue) test failed" << std::endl;
        return false;
    }

    // 测试 as<shared_ptr<int>>() 右值
    std::shared_ptr<int> ptr_int2 = json::value(99).as<std::shared_ptr<int>>();
    if (ptr_int2 == nullptr || *ptr_int2 != 99) {
        std::cerr << "as<shared_ptr<int>>() (rvalue) test failed" << std::endl;
        return false;
    }

    // 测试 null 值的 as<shared_ptr<int>>()
    json::value v_null;
    std::shared_ptr<int> ptr_null = v_null.as<std::shared_ptr<int>>();
    if (ptr_null != nullptr) {
        std::cerr << "as<shared_ptr<int>>() should return nullptr for null value" << std::endl;
        return false;
    }

    // 测试 as<unique_ptr<int>>() 左值
    json::value v_int3 = 33;
    std::unique_ptr<int> uptr_int = v_int3.as<std::unique_ptr<int>>();
    if (uptr_int == nullptr || *uptr_int != 33) {
        std::cerr << "as<unique_ptr<int>>() (lvalue) test failed" << std::endl;
        return false;
    }

    // 测试 as<unique_ptr<int>>() 右值
    std::unique_ptr<int> uptr_int2 = json::value(77).as<std::unique_ptr<int>>();
    if (uptr_int2 == nullptr || *uptr_int2 != 77) {
        std::cerr << "as<unique_ptr<int>>() (rvalue) test failed" << std::endl;
        return false;
    }

    // 测试 null 值的 as<unique_ptr<int>>()
    std::unique_ptr<int> uptr_null = v_null.as<std::unique_ptr<int>>();
    if (uptr_null != nullptr) {
        std::cerr << "as<unique_ptr<int>>() should return nullptr for null value" << std::endl;
        return false;
    }

    // 测试 as<shared_ptr<string>>()
    json::value v_str = "hello";
    std::shared_ptr<std::string> ptr_str = v_str.as<std::shared_ptr<std::string>>();
    if (ptr_str == nullptr || *ptr_str != "hello") {
        std::cerr << "as<shared_ptr<string>>() test failed" << std::endl;
        return false;
    }

    // 测试 as<unique_ptr<string>>()
    json::value v_str2 = "world";
    std::unique_ptr<std::string> uptr_str = v_str2.as<std::unique_ptr<std::string>>();
    if (uptr_str == nullptr || *uptr_str != "world") {
        std::cerr << "as<unique_ptr<string>>() test failed" << std::endl;
        return false;
    }

    std::cout << "smart pointer as<> methods test passed" << std::endl;
    return true;
}

bool test_smart_ptr_complex_types()
{
    std::cout << "Testing smart pointers with complex types..." << std::endl;

    // 测试 shared_ptr<vector<int>> 构造
    std::shared_ptr<std::vector<int>> ptr_vec = std::make_shared<std::vector<int>>(std::vector<int> { 1, 2, 3 });
    json::value v_vec = ptr_vec;
    if (!v_vec.is_array() || v_vec.as_array().size() != 3) {
        std::cerr << "shared_ptr<vector<int>> constructor failed" << std::endl;
        return false;
    }

    // 测试 json array 转换为 shared_ptr<vector<int>>
    json::value v_arr = json::array { 4, 5, 6 };
    std::shared_ptr<std::vector<int>> ptr_vec2 = v_arr.as<std::shared_ptr<std::vector<int>>>();
    if (ptr_vec2 == nullptr || ptr_vec2->size() != 3 || (*ptr_vec2)[0] != 4) {
        std::cerr << "json array to shared_ptr<vector<int>> conversion failed" << std::endl;
        return false;
    }

    // 测试 unique_ptr<vector<int>> 构造
    std::unique_ptr<std::vector<int>> uptr_vec = std::make_unique<std::vector<int>>(std::vector<int> { 7, 8, 9 });
    json::value v_vec2 = std::move(uptr_vec);
    if (!v_vec2.is_array() || v_vec2.as_array().size() != 3) {
        std::cerr << "unique_ptr<vector<int>> constructor failed" << std::endl;
        return false;
    }

    // 测试 json array 转换为 unique_ptr<vector<int>>
    json::value v_arr2 = json::array { 10, 11, 12 };
    std::unique_ptr<std::vector<int>> uptr_vec2 = v_arr2.as<std::unique_ptr<std::vector<int>>>();
    if (uptr_vec2 == nullptr || uptr_vec2->size() != 3 || (*uptr_vec2)[0] != 10) {
        std::cerr << "json array to unique_ptr<vector<int>> conversion failed" << std::endl;
        return false;
    }

    // 测试与 std::optional 结合使用
    json::value v_opt_int = std::make_shared<int>(42);
    if (!v_opt_int.is<std::shared_ptr<int>>()) {
        std::cerr << "is<shared_ptr<int>>() failed after construction from shared_ptr" << std::endl;
        return false;
    }

    // 测试 is<> 对复杂类型的支持
    if (!v_vec.is<std::shared_ptr<std::vector<int>>>()) {
        std::cerr << "is<shared_ptr<vector<int>>>() test failed" << std::endl;
        return false;
    }

    if (!v_vec2.is<std::unique_ptr<std::vector<int>>>()) {
        std::cerr << "is<unique_ptr<vector<int>>>() test failed" << std::endl;
        return false;
    }

    std::cout << "smart pointers with complex types test passed" << std::endl;
    return true;
}

