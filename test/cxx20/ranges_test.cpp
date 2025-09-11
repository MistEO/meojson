#include "ranges_test.h"

#include "json.hpp"

#include <iostream>
#include <ranges>

bool ranges_test()
{
    json::object obj = {
        { "abc", 123 },
        { "def", true },
    };

    for (const auto& key : obj | std::views::keys) {
        std::cout << key << std::endl;
    }
    for (const auto& val : obj | std::views::values) {
        std::cout << val << std::endl;
    }

    return true;
}
