// JSON simple example
// This example does not handle errors.

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

using namespace rapidjson;

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

    std::cout << "string length: " << content.size() << std::endl;

    Document d;
    auto b_time = get_time();

    for (int i = 0; i != loop_times; ++i)
    {
        d.Parse(content.c_str());
    }
    auto e_time = get_time();
    long long avg_time = (e_time - b_time) / loop_times;

    std::cout << "RapidJson parse " << loop_times << " times, avg time: " << avg_time << " ns, about " << avg_time / 1000000.0 << " ms" << std::endl;
    return 0;
}
