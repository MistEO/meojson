#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <sstream>

#include "json.hpp"

int main(int argc, char** argv)
{
    std::string filename;
    size_t loop_times = 1;
    switch (argc) {
    case 3:
        loop_times = std::stoull(argv[2]);
        [[fallthrough]];
    case 2:
        filename = argv[1];
        break;
    default:
        std::cerr << "Usage: ./benchmark <FILE> [TIMES]" << std::endl;
        return -1;
    }

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "open file \"" << filename << "\" failed" << std::endl;
        return -1;
    }
    std::stringstream iss;
    iss << ifs.rdbuf();
    ifs.close();
    const std::string content = iss.str();

    const auto start = std::chrono::steady_clock::now();

    for (size_t i = 0; i != loop_times; ++i) {
        auto value = json::parse(content).value();
    }

    const auto duration = std::chrono::steady_clock::now() - start;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << std::endl;

    return 0;
}
