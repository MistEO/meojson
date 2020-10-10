#include <iostream>
#include <fstream>
#include <sstream>

#include "json_parser.h"

int main()
{
    std::ifstream ifs("test.json");
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    std::cout << content << std::endl;

    bool parse_ret = json::parser::parse(content);
    std::cout << "parse ret : " << (parse_ret ? "true" : "false") << std::endl;

    return 0;
}