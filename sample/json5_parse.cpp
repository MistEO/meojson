
#include <iostream>

#include "json.hpp"

int main()
{
    /*** Parse ***/

    std::cout << "****** Parsing ****** " << std::endl;
    std::string content = R"(
// 这是一段json5格式的信息
{
  名字: "MistEO",                   /* key的引号可省略 */
  😊: '😄',                         // emoji为key
  thanks: 'ありがとう',              /* 单引号也可以表示字符串 */
  \u006Bey: ['value',],            // 普通字符和转义可以混用
  inf: +Infinity, nan: NaN,        // 数字可以以"+"开头
  fractional: .3, integer: 42.,    // 小数点作为起始/结尾
  byte_max: 0xff,                  // 十六进制数
  light_speed: +3e8,               // 科学计数法
}
)";
    try {
        auto ret = json::parse5(content);
        auto value = ret.value();  // As also, you can use rvalues, like `auto
                                   // value = std::move(ret).value();`
        // Output "MistEO"
        std::cout << value["名字"].as_string() << std::endl;
        // Output "value"
        std::cout << value["key"][0].as_string() << std::endl;
    }
    catch (json::parser5::exception& ex) {
        std::cout << "-------parse failed-------" << std::endl;
        std::cerr << ex.what() << std::endl;
    }

    std::string error_content = "{ error }";
    try {
        json::parser5::parse(error_content);
    }
    catch (json::parser5::exception& ex) {
        std::cout << "-------parse failed-------" << std::endl;
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
