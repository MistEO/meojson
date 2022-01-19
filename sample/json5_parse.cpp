
#include <iostream>

#include "json.hpp"

int main() {
  /*** Parse ***/
  {
    std::cout << "****** Parsing ****** " << std::endl;
    std::string content = R"({
     // 中文注释
     中文key: {name: 'meojson',"list": [1, 2, 3]},
      /* 中文块注释
       * 第二行注释
       */
     😊: '' // emoji key
})";
    // std::cout << content << std::endl;
    try {
      auto ret   = json::parser5::parse(content);
      auto value = ret.value();  // As also, you can use rvalues, like `auto
                                 // value = std::move(ret).value();`
      // Output "meojson"
      std::cout << value["中文key"]["name"].as_string() << std::endl;
      // Output 2
      std::cout << value["中文key"]["list"][1].as_integer() << std::endl;
    } catch (json::parser5::exception& ex) {
      std::cout << "-------parse failed-------" << std::endl;
      std::cerr << ex.what() << std::endl;
    }
  }

  std::cout << std::endl;

  return 0;
}
