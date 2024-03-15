#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

#include "json.hpp"
#include "json5.hpp"

template <typename parser>
void do_benchmark(const std::string& content, const std::string& tag)
{
    std::vector<double> iteration_time_ms;
    iteration_time_ms.reserve(10000);
    auto start = std::chrono::steady_clock::now();
    bool parsed = false;
    while (true) {
        auto t0 = std::chrono::steady_clock::now();
        auto opt = parser::parse(content);
        parsed = opt.has_value();

        auto t1 = std::chrono::steady_clock::now();
        iteration_time_ms.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
        if (t1 - start > std::chrono::seconds(3) && iteration_time_ms.size() >= 500) {
            break;
        }
    }

    auto loop_times = iteration_time_ms.size();
    auto sum = std::accumulate(iteration_time_ms.begin(), iteration_time_ms.end(), 0.0);
    double mean = sum / loop_times;
    double stdev_cum = std::accumulate(
        iteration_time_ms.begin(),
        iteration_time_ms.end(),
        0.0,
        [mean](auto a, auto b) {
            auto off = b - mean;
            return a + off * off;
        });

    double stdev = std::sqrt(stdev_cum / loop_times);
    std::sort(iteration_time_ms.begin(), iteration_time_ms.end());

    double median = 0.0;
    if (loop_times % 2 == 0) {
        median = (iteration_time_ms[loop_times / 2 - 1] + iteration_time_ms[loop_times / 2]) / 2;
    }
    else {
        median = iteration_time_ms[loop_times / 2];
    }

    std::cout << tag << ", " << std::boolalpha << parsed << ", " << loop_times << ", " << mean
              << ", " << median << ", " << stdev << std::endl;
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cerr << "Usage: ./benchmark <FILES> ..." << std::endl;
        return -1;
    }

    std::cout << "file, parser, ret, sample_count, mean, median, stdev" << std::endl;

    for (int i = 1; i < argc; ++i) {
        std::filesystem::path path(argv[i]);
        std::ifstream ifs(path);

        if (!ifs.is_open()) {
            std::cerr << "open file \"" << path << "\" failed" << std::endl;
            continue;
        }

        std::stringstream iss;
        iss << ifs.rdbuf();
        ifs.close();
        const std::string& content = iss.str();

        using namespace json::_packed_bytes;

        do_benchmark<json::parser<std::string, std::string, packed_bytes_trait_none>>(
            content,
            path.filename().string() + ", none");
        do_benchmark<json::parser<std::string, std::string, packed_bytes_trait_uint32>>(
            content,
            path.filename().string() + ", bits32");
        do_benchmark<json::parser<std::string, std::string, packed_bytes_trait_uint64>>(
            content,
            path.filename().string() + ", bits64");

        if constexpr (packed_bytes_trait<16>::available) {
            do_benchmark<json::parser<std::string, std::string, packed_bytes_trait<16>>>(
                content,
                path.filename().string() + ", simd128");
        }
        if constexpr (packed_bytes_trait<32>::available) {
            do_benchmark<json::parser<std::string, std::string, packed_bytes_trait<32>>>(
                content,
                path.filename().string() + ", simd256");
        }

        do_benchmark<json::parser5<std::string>>(content, path.filename().string() + ", json5");
    }

    return 0;
}
