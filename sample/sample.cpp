#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/time.h>
#include <ctime>

#include "json.h"

int main()
{
    /*** Parse: String to Json ***/
    std::ifstream ifs("test.json");
    std::stringstream ibuf;
    ibuf << ifs.rdbuf();
    std::string content(ibuf.str());
    std::cout << content << std::endl;

    json::value val;

    auto get_time = []() -> long long {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    };

    auto b_time = get_time();

    for (int i = 0; i != 10000; ++i)
    {
        val = json::parser::parse(content);
    }
    auto e_time = get_time();

    std::cout << "parse success" << std::endl;
    std::cout << (e_time - b_time) << " ns" << std::endl;

    std::cout << val.as_object()["configurations"].as_array()[0].as_object()["name"].as_string() << std::endl;

    /*** Ify: Json to String  ***/
    json::object obj;
    obj["key1"] = json::value::string("hello");
    obj["PI"] = json::value::number(3.1416);
    std::vector<json::value> value_vector;
    for (int i = 0; i != 10; ++i)
    {
        value_vector.push_back(json::value::number(i));
    }
    obj["list"] = json::array(value_vector);

    json::object obj2;
    obj2["obj2_key1"] = json::value::string("i am child");
    obj["child"] = obj2;

    std::cout << obj.to_string() << std::endl;

    return 0;
}
