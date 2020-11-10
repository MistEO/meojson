#include <iostream>
#include <fstream>
#include <sstream>

#include <chrono>

#include "json.h"

constexpr int loop_times = 10;
constexpr const char *filename = "rand.json";

int main()
{
    auto get_time = []() -> long long {
        return std::chrono::system_clock::now().time_since_epoch().count();
    };

    std::ifstream ifs(filename);
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    //std::cout << content << std::endl;

    json::value val;

    auto b_time = get_time();

    for (int i = 0; i != loop_times; ++i)
    {
        val = json::parser::parse(content).second;
    }
    auto e_time = get_time();
    long long dis = (e_time - b_time);

    std::cout << "parse success, pre time: " << dis / loop_times << " ns" << std::endl;

    return 0;
}
