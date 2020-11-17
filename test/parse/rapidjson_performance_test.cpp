#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include "rapidjson/document.h"

int main(int argc, char *argv[])
{
    const char *filename = "rand.json";
    int loop_times = 10;
    if (argc == 3)
    {
        filename = argv[1];
        loop_times = atoi(argv[2]);
    }
    std::cout << "parse file: " << filename << ", loop times: " << loop_times << std::endl;

    auto get_time = []() -> long long {
        return std::chrono::system_clock::now().time_since_epoch().count();
    };

    std::ifstream ifs(filename);
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    //std::cout << content << std::endl;

    std::cout << "string size: " << content.size() << std::endl;

    rapidjson::Document d;

    auto b_time = get_time();
    for (int i = 0; i != loop_times; ++i)
    {
        d.Parse(content.c_str());
    }
    auto e_time = get_time();

    long long total_time = e_time - b_time;
    long long avg_time = total_time / loop_times;
    const double ns_to_s = 1000 * 1000 * 1000;
    std::cout << "RapidJson parse finished\n"
              << "total time:\t" << total_time << " ns,\tabout\t" << total_time / ns_to_s << " s\n"
              << "avg time:\t" << avg_time << " ns,\tabout\t" << avg_time / ns_to_s << " s" << std::endl;
    return 0;
}
