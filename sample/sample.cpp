#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/time.h>
#include <ctime>

#include "json.h"

int main()
{
    auto get_time = []() -> long long {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    };

    /*** Parse: String to Json ***/
    {
        std::ifstream ifs("test.json");
        std::stringstream ibuf;
        ibuf << ifs.rdbuf();
        std::string content(ibuf.str());
        std::cout << content << std::endl;

        json::value val;
        const int loop_times = 100000;

        auto b_time = get_time();

        for (int i = 0; i != loop_times; ++i)
        {
            val = json::parser::parse(content);
        }
        auto e_time = get_time();
        long long dis = (e_time - b_time);

        std::cout << "parse success, pre time: " << dis / loop_times << " ns" << std::endl;
    }

    /*** Ify: Json to String  ***/
    {
        auto b_time = get_time();
        const int loop_times = 100000;
        json::object obj;
        for (int i = 0; i != loop_times; ++i)
        {
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
            json::array arr = {json::value::string("123"), json::value::number(456)};
            obj["arr"] = arr;
        }
        auto e_time = get_time();
        auto dis = (e_time - b_time);

        std::cout << "ify success, pre time: " << dis / loop_times << " ns" << std::endl;
        std::cout << obj.to_string() << std::endl;
    }

    return 0;
}
