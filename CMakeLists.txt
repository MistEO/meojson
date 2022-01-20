cmake_minimum_required(VERSION 3.0)
project(meojson)

include_directories(./include)

option(BUILD_SAMPLE "Build sample in sample/sample.cpp" ON)

# add_compile_options(-Wall -std=c++17 -O2)
# set(CMAKE_BUILD_TYPE Release) # 自带-O2选项
set(CMAKE_CXX_STANDARD 17)    # std=c++17

if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

# add_library(meojson STATIC ${SRC})

if (BUILD_SAMPLE)
  add_executable(sample sample/sample.cpp)
  add_executable(json5_parse_sample sample/json5_parse.cpp)
endif()

# target_link_libraries(sample.out meojson ${CMAKE_THREAD_LIBS_INIT})
