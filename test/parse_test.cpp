#include <iostream>

#include "json.hpp"
#include "parse_test.h"

bool normal_json();
bool wrong_json();
bool jsonc_trail_comma();
bool jsonc_comment();
bool string_control_characters();

bool parsing()
{
    return normal_json() && wrong_json() && jsonc_trail_comma() && jsonc_comment() && string_control_characters();
}

bool normal_json()
{
    std::string j = R"({})";

    if (!json::parse(j)) {
        std::cerr << "Parsing failed:" << j << std::endl;
        return false;
    }
    return true;
}

bool wrong_json()
{
    std::string duplicate_key_json = R"({"key": 1, "key": 2})";
    if (json::parse(duplicate_key_json)) {
        std::cerr << "Parsing failed:" << duplicate_key_json << std::endl;
        return false;
    }

    std::string wrong_json_with_comments = R"({\)";
    if (json::parse(wrong_json_with_comments)) {
        std::cerr << "Parsing failed:" << wrong_json_with_comments << std::endl;
        return false;
    }
    std::string wrong_json_with_comments2 = R"({\**)";
    if (json::parse(wrong_json_with_comments2)) {
        std::cerr << "Parsing failed:" << wrong_json_with_comments2 << std::endl;
        return false;
    }

    return true;
}

bool string_control_characters()
{
    std::string raw_control = std::string("{\"key\":\"") + char(1) + "\"}";
    if (json::parse(raw_control)) {
        std::cerr << "Parsing should reject raw control character in string" << std::endl;
        return false;
    }

    std::string raw_newline = std::string("{\"key\":\"") + '\n' + "\"}";
    if (json::parse(raw_newline)) {
        std::cerr << "Parsing should reject raw newline in string" << std::endl;
        return false;
    }

    std::string raw_tab = std::string("{\"key\":\"") + '\t' + "\"}";
    if (json::parse(raw_tab)) {
        std::cerr << "Parsing should reject raw tab in string" << std::endl;
        return false;
    }

    std::string raw_carriage_return = std::string("{\"key\":\"") + '\r' + "\"}";
    if (json::parse(raw_carriage_return)) {
        std::cerr << "Parsing should reject raw carriage return in string" << std::endl;
        return false;
    }

    auto escaped_control = json::parse(R"({"key":"\u0001"})");
    if (!escaped_control) {
        std::cerr << "Parsing should accept escaped control character" << std::endl;
        return false;
    }
    if (escaped_control->at("key").as_string() != std::string(1, char(1))) {
        std::cerr << "Escaped control character decoded incorrectly" << std::endl;
        return false;
    }

    auto escaped_common = json::parse(R"({"newline":"\n","tab":"\t","carriage_return":"\r"})");
    if (!escaped_common) {
        std::cerr << "Parsing should accept escaped common control characters" << std::endl;
        return false;
    }
    if (escaped_common->at("newline").as_string() != std::string(1, '\n')) {
        std::cerr << "Escaped newline decoded incorrectly" << std::endl;
        return false;
    }
    if (escaped_common->at("tab").as_string() != std::string(1, '\t')) {
        std::cerr << "Escaped tab decoded incorrectly" << std::endl;
        return false;
    }
    if (escaped_common->at("carriage_return").as_string() != std::string(1, '\r')) {
        std::cerr << "Escaped carriage return decoded incorrectly" << std::endl;
        return false;
    }

    std::string serialized = escaped_common->to_string();
    if (serialized.find('\n') != std::string::npos || serialized.find('\t') != std::string::npos
        || serialized.find('\r') != std::string::npos) {
        std::cerr << "Serialized JSON should not contain raw common control characters" << std::endl;
        return false;
    }

    return true;
}

bool jsonc_trail_comma()
{
    std::string obj_trail = R"({ "key": 1, })";
    if (!json::parsec(obj_trail)) {
        std::cerr << "Parsing failed:" << obj_trail << std::endl;
        return false;
    }

    std::string arr_trail = R"([ 1, 2, ])";
    if (!json::parsec(arr_trail)) {
        std::cerr << "Parsing failed:" << arr_trail << std::endl;
        return false;
    }

    return true;
}

bool jsonc_comment()
{
    std::string json = R"(
/*123//321
123*/{ "key"://456/*789
 1, }
/* trail */
// 111
)";
    if (!json::parsec(json)) {
        std::cerr << "Parsing failed:" << json << std::endl;
        return false;
    }

    std::string json2 = R"({}//)";

    if (!json::parsec(json2)) {
        std::cerr << "Parsing failed:" << json2 << std::endl;
        return false;
    }
    return true;
}
