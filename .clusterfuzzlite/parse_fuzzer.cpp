#include <string_view>

#include "json.hpp"
#include "json5.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    std::string_view content(reinterpret_cast<const char*>(data), size);
    json::parse(content);
    json::parse5(content);
    return 0;
}
