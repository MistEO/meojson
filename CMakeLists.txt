cmake_minimum_required(VERSION 3.0)
project(meojson)

include_directories(./include)

option(BUILD_SAMPLE "Build sample in sample/sample.cpp" ON)
option(BUILD_BENCHMARK "Build benchmark in benchmark/benchmark.cpp" OFF)

set(CMAKE_CXX_STANDARD 17)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/W4;/WX>")
add_compile_options("$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-Wextra;-Wpedantic;-Werror>")

if (BUILD_SAMPLE)
    add_executable(sample sample/sample.cpp)
    add_executable(json5_parse_sample sample/json5_parse.cpp)
endif()

if (BUILD_BENCHMARK)
    SET(CMAKE_BUILD_TYPE "Release")
    add_executable(benchmark benchmark/benchmark.cpp)
endif()