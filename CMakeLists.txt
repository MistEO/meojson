cmake_minimum_required(VERSION 3.0)
project(meojson)

aux_source_directory(src SRC)
include_directories(./include)

option(BUILD_SAMPLE "Build sample in sample/sample.cpp" ON)

# add_compile_options(-Wall -std=c++17 -O2)
find_package(Threads)
# set(CMAKE_BUILD_TYPE Release) # 自带-O2选项
set(CMAKE_CXX_STANDARD 17)    # std=c++17

if (WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/MP>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/MP>")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

# add_library(meojson STATIC ${SRC})

if (BUILD_SAMPLE)
  add_executable(sample sample/sample.cpp)
  add_executable(json5_parse_sample sample/json5_parse.cpp)
endif()

# target_link_libraries(sample.out meojson ${CMAKE_THREAD_LIBS_INIT})