#include <iostream>

#include "json5.hpp"

bool parsing();
bool parsing_error();

int main()
{
    /*** Parsing Json5 ***/
    std::cout << "\n****** Parsing 5 ******\n" << std::endl;

    if (!parsing()) {
        return -1;
    }

    std::cout << "\n****** Parsing error 5 ******\n" << std::endl;

    if (!parsing_error()) {
        return -1;
    }

    return 0;
}

bool parsing()
{
    auto content_raw = R"(
// 这是一段json5格式的信息
{
  名字: "MistEO",                  /* key的引号可省略 */
  😊: '😄',                       // emoji为key
  thanks: 'ありがとう',             /* 单引号也可以表示字符串 */
  \u006Bey: ['value',],            // 普通字符和转义可以混用
  inf: +Infinity, nan: NaN,        // 数字可以以"+"开头
  fractional: .3, integer: 42.,    // 小数点作为起始/结尾
  byte_max: 0xff,                  // 十六进制数
  light_speed: +3e8,               // 科学计数法
}
)";
    std::string_view content = content_raw;
    auto ret = json::parse5(content);
    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return false;
    }
    {     
        auto another_ret = json::parse5(content);
        if (!another_ret) {
            std::cerr << "Parsing failed" << std::endl;
            return false;
        }//C-style strings can also be used as input.
    }
    auto& value = ret.value(); // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`

    // Output: MistEO
    std::cout << value["名字"].as_string() << std::endl;
    // Output: value
    std::string str = (std::string)value["key"][0];
    std::cout << str << std::endl;

    // for more json::value usage, please refer to sample.cpp
    return true;
}

bool parsing_error()
{
    std::string error_content = "{ error }";
    std::string error_msg;
    auto err_ret = json::parse5(error_content, &error_msg);
    if (!err_ret) {
        std::cout << "Parsing failed" << std::endl;
        std::cout << error_msg << std::endl;
        return true;
    }

    return false;
}
