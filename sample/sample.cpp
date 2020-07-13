#include "json.h"

int main()
{
    std::string json_string = "{\"str\": \"ABC\", \"num\": 1}";
    json::object json;
    json.parse(json_string);
    return 0;
}