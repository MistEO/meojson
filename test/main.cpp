#include <array>
#include <filesystem>
#include <iostream>
#include <list>
#include <set>
#include <unordered_map>

#include "json.hpp"

int main()
{
    std::cout << "\n****** Testing ******\n" << std::endl;

    json::value root;

    root["hello"] = "meojson";
    root["Pi"] = 3.1416;

    root["obj"] = {
        { "obj_key1", "Hi" },
        { "obj_key2", 123 },
        { "obj_key3", true },
    };
    root["obj"].emplace("obj_key4", 789);

    root["obj"].emplace("obj_key5", json::object { { "key4 child", "i am object value" } });
    root["another_obj"]["child"]["grand"] = "i am grand";

    // take union
    root["obj"] |= json::object {
        { "obj_key6", "i am string" },
        { "obj_key7", json::array { "i", "am", "array" } },
    };

    root["arr"] = json::array { 1, 2, 3 };
    root["arr"].emplace(4);
    root["arr"].emplace(5);
    root["arr"] += json::array { 6, 7 };

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = vec;

    std::set<std::string> set = { "a", "bb\n\nb", "cc\t" };
    root["arr from set"] = set;

    std::map<std::string, int> map {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = map;

    std::vector<std::list<std::set<int>>> complex { { { 1, 2, 3 }, { 4, 5 } }, { { 6 }, { 7, 8 } } };
    root["complex"] = json::serialize<false>(complex);

    std::map<std::array<int, 3>, std::map<int, std::array<double, 2>>> more_complex {
        { { 1, 2, 3 }, { { 1, { 0.1, 0.2 } }, { 2, { 0.2, 0.3 } } } },
        { { 4, 5, 6 }, { { 3, { 0.4 } }, { 4, { 0.5, 0.6 } } } },
    };

    std::filesystem::path path = "path/to/file";
    root["path"] = json::serialize<true>(path);
    std::vector<std::filesystem::path> paths = { "path/to/file1", "path/to/file2" };
    root["paths"] = json::serialize<true>(paths);

    root["more_complex"] = json::serialize<true>(more_complex);
    if (root["more_complex"].to_string() !=
        R"({"[1,2,3]":{"1":[0.100000,0.200000],"2":[0.200000,0.300000]},"[4,5,6]":{"3":[0.400000,0.000000],"4":[0.500000,0.600000]}})") {
        std::cerr << "error: " << root["more_complex"].to_string() << std::endl;
        return -1;
    }

    root["a\\n"] = "1a\\n";
    root["a\n"] = "2a\n";
    if (root["a\\n"].as_string() != "1a\\n") {
        std::cerr << "error: " << root["a\\n"].as_string() << std::endl;
        return -1;
    }
    if (root["a\n"].as_string() != "2a\n") {
        std::cerr << "error: " << root["a\n"].as_string() << std::endl;
        return -1;
    }
    if (root["a\\n"].to_string() != "\"1a\\\\n\"") {
        std::cerr << "error: " << root["a\\n"].to_string() << std::endl;
        return -1;
    }
    if (root["a\n"].to_string() != "\"2a\\n\"") {
        std::cerr << "error: " << root["a\n"].to_string() << std::endl;
        return -1;
    }

    std::cout << root << std::endl;
    std::ofstream ofs("test.json");
    ofs << root;
    ofs.close();

    std::cout << "\n****** Test passed ******\n" << std::endl;
    return 0;
}
