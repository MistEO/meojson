#include <array>
#include <iostream>

#include "json.hpp"

// constexpr json::object wrap_obj(json::value v)
// {
//     return json::object { { "kkk", v } };
// }

// constexpr json::value empty;
// constexpr json::object wempty = wrap_obj(empty);

constexpr size_t test()
{
    json::basic_object<std::string, json::constexpr_map> empty = { { "abc", 1 } };

    empty["def"] = 123;
    empty["float"] = -1234.567890987654321;
    empty.emplace("float", 1);

    return empty.to_string().size();
}

template <size_t sz>
constexpr std::array<char, sz + 1> test2()
{
    json::basic_object<std::string, json::constexpr_map> empty = { { "abc", 1 } };

    empty["def"] = 123;
    empty["float"] = -1234.567890987654321;
    empty.emplace("float", 1);

    std::array<char, sz + 1> buf;
    auto str = empty.to_string();
    for (size_t i = 0; i < sz; i++) {
        buf[i] = str[i];
    }
    buf[sz] = 0;
    return buf;
}

int main()
{
    constexpr auto v = test();
    constexpr auto str = test2<v>();
    std::cout << str.data() << std::endl;
}