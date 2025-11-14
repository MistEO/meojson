#include <string_view>

#include "json.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::string_view content(reinterpret_cast<const char*>(data), size);
    json::parse(content);
    json::parsec(content);
    return 0;
}
