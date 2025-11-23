#include <iostream>

#include "json.hpp"
#include "jsonc_test.h"

bool test_jsonc_comments();
bool test_jsonc_trailing_commas();
bool test_jsonc_extended_syntax();
bool test_complex_serialization();
bool test_complex_deserialization();
bool test_edge_cases();

bool jsonc_test()
{
    return test_jsonc_comments() && test_jsonc_trailing_commas() && test_jsonc_extended_syntax()
           && test_complex_serialization() && test_complex_deserialization() && test_edge_cases();
}

bool test_jsonc_comments()
{
    std::cout << "Testing JSONC comments..." << std::endl;

    // 单行注释
    std::string json_with_line_comment = R"(
        {
            // This is a comment
            "key1": "value1",
            "key2": 42 // Another comment
        }
    )";

    auto result1 = json::parsec(json_with_line_comment);
    if (!result1.has_value() || !result1->is_object()) {
        std::cerr << "Failed to parse JSONC with line comments" << std::endl;
        return false;
    }
    if (result1->as_object()["key1"].as_string() != "value1" || result1->as_object()["key2"].as_integer() != 42) {
        std::cerr << "JSONC line comment parsing produced incorrect values" << std::endl;
        return false;
    }

    // 块注释
    std::string json_with_block_comment = R"(
        {
            /* This is a
               multi-line comment */
            "key1": "value1",
            "key2": /* inline comment */ 42
        }
    )";

    auto result2 = json::parsec(json_with_block_comment);
    if (!result2.has_value() || !result2->is_object()) {
        std::cerr << "Failed to parse JSONC with block comments" << std::endl;
        return false;
    }

    // 混合注释
    std::string json_with_mixed_comments = R"(
        /* Header comment */
        {
            "key1": "value1", // Line comment
            /* Block comment */ "key2": 42
        }
        // Trailing comment
    )";

    auto result3 = json::parsec(json_with_mixed_comments);
    if (!result3.has_value() || !result3->is_object()) {
        std::cerr << "Failed to parse JSONC with mixed comments" << std::endl;
        return false;
    }

    std::cout << "JSONC comments test passed" << std::endl;
    return true;
}

bool test_jsonc_trailing_commas()
{
    std::cout << "Testing JSONC trailing commas..." << std::endl;

    // 对象尾随逗号
    std::string obj_with_trailing = R"({
        "key1": "value1",
        "key2": "value2",
    })";

    auto result1 = json::parsec(obj_with_trailing);
    if (!result1.has_value() || !result1->is_object()) {
        std::cerr << "Failed to parse object with trailing comma" << std::endl;
        return false;
    }
    if (result1->as_object().size() != 2) {
        std::cerr << "Object with trailing comma has incorrect size" << std::endl;
        return false;
    }

    // 数组尾随逗号
    std::string arr_with_trailing = R"([
        1,
        2,
        3,
    ])";

    auto result2 = json::parsec(arr_with_trailing);
    if (!result2.has_value() || !result2->is_array()) {
        std::cerr << "Failed to parse array with trailing comma" << std::endl;
        return false;
    }
    if (result2->as_array().size() != 3) {
        std::cerr << "Array with trailing comma has incorrect size" << std::endl;
        return false;
    }

    // 嵌套结构的尾随逗号
    std::string nested_with_trailing = R"({
        "array": [1, 2, 3,],
        "object": {"a": 1, "b": 2,},
    })";

    auto result3 = json::parsec(nested_with_trailing);
    if (!result3.has_value() || !result3->is_object()) {
        std::cerr << "Failed to parse nested structure with trailing commas" << std::endl;
        return false;
    }

    std::cout << "JSONC trailing commas test passed" << std::endl;
    return true;
}

bool test_jsonc_extended_syntax()
{
    std::cout << "Testing JSONC extended syntax..." << std::endl;

    // 测试parsec是否正确处理标准JSON
    std::string standard_json = R"({
        "key1": "value1",
        "key2": 42,
        "key3": [1, 2, 3]
    })";

    auto result = json::parsec(standard_json);
    if (!result.has_value()) {
        std::cerr << "parsec should handle standard JSON" << std::endl;
        return false;
    }

    // 测试parse不接受JSONC特性
    std::string jsonc_str = R"({"key": 1,})";
    auto standard_result = json::parse(jsonc_str);
    if (standard_result.has_value()) {
        std::cerr << "parse should not accept trailing commas" << std::endl;
        return false;
    }

    // 但parsec应该接受
    auto jsonc_result = json::parsec(jsonc_str);
    if (!jsonc_result.has_value()) {
        std::cerr << "parsec should accept trailing commas" << std::endl;
        return false;
    }

    std::cout << "JSONC extended syntax test passed" << std::endl;
    return true;
}

bool test_complex_serialization()
{
    std::cout << "Testing complex serialization..." << std::endl;

    // 深层嵌套结构
    json::value deep_nested = json::object {
        { "level1",
          json::object {
              { "level2",
                json::object { { "level3",
                                 json::object { { "level4", json::array { 1, 2, 3 } }, { "value", "deep" } } } } } } }
    };

    std::string serialized = deep_nested.to_string();
    auto deserialized = json::parse(serialized);
    if (!deserialized.has_value()) {
        std::cerr << "Failed to deserialize deep nested structure" << std::endl;
        return false;
    }

    // 验证深层嵌套数据
    std::string deep_value = deserialized->get("level1", "level2", "level3", "value", std::string(""));
    if (deep_value != "deep") {
        std::cerr << "Deep nested value incorrect after round-trip" << std::endl;
        return false;
    }

    // 大型数组
    json::array large_array;
    for (int i = 0; i < 1000; i++) {
        large_array.emplace_back(i);
    }
    json::value large_arr_val = large_array;

    std::string large_serialized = large_arr_val.to_string();
    auto large_deserialized = json::parse(large_serialized);
    if (!large_deserialized.has_value() || large_deserialized->as_array().size() != 1000) {
        std::cerr << "Large array round-trip failed" << std::endl;
        return false;
    }

    // 大型对象
    json::object large_object;
    for (int i = 0; i < 100; i++) {
        large_object.emplace("key" + std::to_string(i), i);
    }
    json::value large_obj_val = large_object;

    std::string large_obj_serialized = large_obj_val.to_string();
    auto large_obj_deserialized = json::parse(large_obj_serialized);
    if (!large_obj_deserialized.has_value() || large_obj_deserialized->as_object().size() != 100) {
        std::cerr << "Large object round-trip failed" << std::endl;
        return false;
    }

    // 混合类型复杂结构
    json::value complex = json::object {
        { "null", nullptr },
        { "bool_true", true },
        { "bool_false", false },
        { "int", 42 },
        { "float", 3.14159 },
        { "string", "hello world" },
        { "empty_string", "" },
        { "empty_array", json::array {} },
        { "empty_object", json::object {} },
        { "array", json::array { 1, "two", 3.0, true, nullptr } },
        { "object", json::object { { "nested", "value" } } }
    };

    std::string complex_serialized = complex.to_string();
    auto complex_deserialized = json::parse(complex_serialized);
    if (!complex_deserialized.has_value()) {
        std::cerr << "Complex structure round-trip failed" << std::endl;
        return false;
    }

    // 验证所有类型
    if (!complex_deserialized->at("null").is_null() || !complex_deserialized->at("bool_true").as_boolean()
        || complex_deserialized->at("bool_false").as_boolean() || complex_deserialized->at("int").as_integer() != 42
        || complex_deserialized->at("string").as_string() != "hello world"
        || !complex_deserialized->at("empty_array").empty() || !complex_deserialized->at("empty_object").empty()) {
        std::cerr << "Complex structure values incorrect after round-trip" << std::endl;
        return false;
    }

    std::cout << "Complex serialization test passed" << std::endl;
    return true;
}

bool test_complex_deserialization()
{
    std::cout << "Testing complex deserialization..." << std::endl;

    // Unicode字符串
    std::string unicode_json = R"({"emoji": "😀", "chinese": "你好", "japanese": "こんにちは"})";
    auto unicode_result = json::parse(unicode_json);
    if (!unicode_result.has_value()) {
        std::cerr << "Failed to parse Unicode JSON" << std::endl;
        return false;
    }

    // 转义字符
    std::string escaped_json = R"({"newline": "line1\nline2", "tab": "col1\tcol2", "quote": "He said \"hello\""})";
    auto escaped_result = json::parse(escaped_json);
    if (!escaped_result.has_value()) {
        std::cerr << "Failed to parse escaped JSON" << std::endl;
        return false;
    }
    if (escaped_result->at("newline").as_string().find('\n') == std::string::npos) {
        std::cerr << "Newline escape not parsed correctly" << std::endl;
        return false;
    }

    // 数字边界情况
    std::string number_json = R"({
        "zero": 0,
        "negative": -42,
        "float": 3.14159,
        "scientific": 1.23e10,
        "negative_scientific": -1.23e-10
    })";
    auto number_result = json::parse(number_json);
    if (!number_result.has_value()) {
        std::cerr << "Failed to parse various number formats" << std::endl;
        return false;
    }

    // 数组中的多种类型
    std::string mixed_array_json = R"([null, true, false, 42, "string", [], {}])";
    auto mixed_array_result = json::parse(mixed_array_json);
    if (!mixed_array_result.has_value() || mixed_array_result->as_array().size() != 7) {
        std::cerr << "Failed to parse mixed type array" << std::endl;
        return false;
    }

    // 验证每个元素的类型
    const auto& arr = mixed_array_result->as_array();
    if (!arr[0].is_null() || !arr[1].is_boolean() || !arr[2].is_boolean() || !arr[3].is_number() || !arr[4].is_string()
        || !arr[5].is_array() || !arr[6].is_object()) {
        std::cerr << "Mixed type array elements have incorrect types" << std::endl;
        return false;
    }

    std::cout << "Complex deserialization test passed" << std::endl;
    return true;
}

bool test_edge_cases()
{
    std::cout << "Testing edge cases..." << std::endl;

    // 空JSON
    auto empty_obj = json::parse("{}");
    if (!empty_obj.has_value() || !empty_obj->is_object() || !empty_obj->empty()) {
        std::cerr << "Empty object parsing failed" << std::endl;
        return false;
    }

    auto empty_arr = json::parse("[]");
    if (!empty_arr.has_value() || !empty_arr->is_array() || !empty_arr->empty()) {
        std::cerr << "Empty array parsing failed" << std::endl;
        return false;
    }

    // 单个值（根据 parser.hpp，根元素必须是 object 或 array，因此这些应该解析失败）
    auto single_null = json::parse("null");
    if (single_null.has_value()) {
        std::cerr << "Single null value should not be parsed" << std::endl;
        return false;
    }

    auto single_bool = json::parse("true");
    if (single_bool.has_value()) {
        std::cerr << "Single boolean value should not be parsed" << std::endl;
        return false;
    }

    auto single_number = json::parse("42");
    if (single_number.has_value()) {
        std::cerr << "Single number value should not be parsed" << std::endl;
        return false;
    }

    auto single_string = json::parse(R"("hello")");
    if (single_string.has_value()) {
        std::cerr << "Single string value should not be parsed" << std::endl;
        return false;
    }

    // 空白字符处理
    auto whitespace_json = json::parse("  \n\t {  \"key\"  :  \"value\"  }  \n\t  ");
    if (!whitespace_json.has_value() || whitespace_json->at("key").as_string() != "value") {
        std::cerr << "Whitespace handling failed" << std::endl;
        return false;
    }

    // 特殊字符串
    json::value special_strings = json::object {
        { "empty", "" },
        { "space", " " },
        { "newline", "\n" },
        { "tab", "\t" },
        { "backslash", "\\" },
        { "quote", "\"" }
    };

    std::string special_serialized = special_strings.to_string();
    auto special_deserialized = json::parse(special_serialized);
    if (!special_deserialized.has_value()) {
        std::cerr << "Special strings round-trip failed" << std::endl;
        return false;
    }
    if (special_deserialized->at("empty").as_string() != "" || special_deserialized->at("newline").as_string() != "\n"
        || special_deserialized->at("backslash").as_string() != "\\" || special_deserialized->at("quote").as_string() != "\"") {
        std::cerr << "Special strings values incorrect after round-trip" << std::endl;
        return false;
    }

    // 重复键（应该失败）
    auto duplicate_keys = json::parse(R"({"key": 1, "key": 2})");
    if (duplicate_keys.has_value()) {
        std::cerr << "Duplicate keys should not be allowed" << std::endl;
        return false;
    }

    // 无效JSON（应该失败）
    auto invalid_json1 = json::parse("{key: value}");  // 无引号的键
    if (invalid_json1.has_value()) {
        std::cerr << "Invalid JSON (unquoted key) should fail" << std::endl;
        return false;
    }

    auto invalid_json2 = json::parse("[1, 2, 3,]");  // 标准JSON不允许尾随逗号
    if (invalid_json2.has_value()) {
        std::cerr << "Invalid JSON (trailing comma) should fail in standard mode" << std::endl;
        return false;
    }

    auto invalid_json3 = json::parse("{\"key\": }");  // 缺少值
    if (invalid_json3.has_value()) {
        std::cerr << "Invalid JSON (missing value) should fail" << std::endl;
        return false;
    }

    // 格式化输出测试
    json::value format_test = json::object { { "a", 1 }, { "b", json::array { 1, 2, 3 } } };

    std::string unformatted = format_test.dumps();
    std::string formatted2 = format_test.dumps(2);
    std::string formatted4 = format_test.format(4);

    if (formatted2.length() <= unformatted.length() || formatted4.length() <= unformatted.length()) {
        std::cerr << "Formatted output should be longer than unformatted" << std::endl;
        return false;
    }

    std::cout << "Edge cases test passed" << std::endl;
    return true;
}

