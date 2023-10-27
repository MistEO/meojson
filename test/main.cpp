#include <array>
#include <filesystem>
#include <iostream>
#include <list>
#include <set>
#include <unordered_map>

#include "json.hpp"

bool serializing();
bool wstring_serializing();

int main()
{
    std::cout << "\n****** Testing ******\n" << std::endl;

    std::cout << "\n*** serializing ***\n" << std::endl;
    bool success = serializing();

    std::cout << "\n*** wstring_serializing ***\n" << std::endl;
    success &= wstring_serializing();

    if (!success) {
        std::cout << "\n****** Test failed ******\n" << std::endl;
        return -1;
    }

    std::cout << "\n****** Test passed ******\n" << std::endl;
    return 0;
}

bool serializing()
{
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

    auto to_arr = root["arr"].to_vector<int>();

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = vec;

    std::set<std::string> set = { "a", "bb\n\nb", "cc\t" };
    root["arr from set"] = set;

    std::map<std::string, int> map {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = map;

    auto to_map = root["obj from map"].to_map<int>();

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
        return false;
    }

    root["a\\n"] = "1a\\n";
    root["a\n"] = "2a\n";
    if (root["a\\n"].as_string() != "1a\\n") {
        std::cerr << "error: " << root["a\\n"].as_string() << std::endl;
        return false;
    }
    if (root["a\n"].as_string() != "2a\n") {
        std::cerr << "error: " << root["a\n"].as_string() << std::endl;
        return false;
    }
    if (root["a\\n"].to_string() != "\"1a\\\\n\"") {
        std::cerr << "error: " << root["a\\n"].to_string() << std::endl;
        return false;
    }
    if (root["a\n"].to_string() != "\"2a\\n\"") {
        std::cerr << "error: " << root["a\n"].to_string() << std::endl;
        return false;
    }

    std::cout << root << std::endl;
    std::ofstream ofs("serializing.json");
    ofs << root;
    ofs.close();

    return true;
}

bool wstring_serializing()
{
    json::wvalue root;

    root[L"hello"] = L"meojson";
    root[L"Pi"] = 3.1416;

    root[L"obj"] = {
        { L"obj_key1", L"Hi" },
        { L"obj_key2", 123 },
        { L"obj_key3", true },
    };
    root[L"obj"].emplace(L"obj_key4", 789);

    root[L"obj"].emplace(L"obj_key5", json::wobject { { L"key4 child", L"i am object value" } });
    root[L"another_obj"][L"child"][L"grand"] = L"i am grand";

    // take union
    root[L"obj"] |= json::wobject {
        { L"obj_key6", L"i am string" },
        { L"obj_key7", json::warray { L"i", L"am", L"array" } },
    };

    root[L"arr"] = json::warray { 1, 2, 3 };
    root[L"arr"].emplace(4);
    root[L"arr"].emplace(5);
    root[L"arr"] += json::warray { 6, 7 };

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root[L"arr from vec"] = vec;

    std::set<std::wstring> set = { L"a", L"bb\n\nb", L"cc\t" };
    root[L"arr from set"] = set;

    std::map<std::wstring, int> map {
        { L"key1", 1 },
        { L"key2", 2 },
    };
    root[L"obj from map"] = map;

    root[L"a\\n"] = L"1a\\n";
    root[L"a\n"] = L"2a\n";
    if (root[L"a\\n"].as_string() != L"1a\\n") {
        std::wcerr << "error: " << root[L"a\\n"].as_string() << std::endl;
        return false;
    }
    if (root[L"a\n"].as_string() != L"2a\n") {
        std::wcerr << "error: " << root[L"a\n"].as_string() << std::endl;
        return false;
    }
    if (root[L"a\\n"].to_string() != L"\"1a\\\\n\"") {
        std::wcerr << "error: " << root[L"a\\n"].to_string() << std::endl;
        return false;
    }
    if (root[L"a\n"].to_string() != L"\"2a\\n\"") {
        std::wcerr << "error: " << root[L"a\n"].to_string() << std::endl;
        return false;
    }

    std::wcout << root << std::endl;
    std::wofstream ofs("wstring_serializing.json");
    ofs << root;
    ofs.close();

    return true;
}
