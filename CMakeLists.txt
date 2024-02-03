cmake_minimum_required(VERSION 3.14)
project(meojson CXX)

include_directories(./include)

option(BUILD_SAMPLE "Build sample in sample/sample.cpp" ON)
option(BUILD_BENCHMARK "Build benchmark in benchmark/benchmark.cpp" OFF)
option(ENABLE_NEON "Enable ARM NEON" OFF)
option(BUILD_TESTING "Build testing" ON)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    add_compile_options("/utf-8" "/W4")
else()
    add_compile_options("-Wall;-Wextra;-Wpedantic;-mtune=native")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options("-Wno-gnu-zero-variadic-macro-arguments")
    endif()
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|EM64T|x86_64")
        add_compile_options("-msse4.1")
    endif()
endif()

if(ENABLE_NEON)
    add_compile_definitions(MEOJSON_ENABLE_NEON)
endif()

file(GLOB_RECURSE json_headers
    include/*.h
    include/*.hpp
)

if (BUILD_SAMPLE)
    file(GLOB sample_src
        sample/*.cpp
        sample/*.h
        sample/*.hpp
    )
    add_executable(sample ${sample_src} ${json_headers})

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${json_headers})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sample_src})
endif()

if (BUILD_TESTING)
    file(GLOB testing_src
        test/*.cpp
        test/*.h
        test/*.hpp
    )
    add_executable(testing ${testing_src} ${json_headers})

    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${json_headers})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${testing_src})
endif()

if (BUILD_BENCHMARK)
    SET(CMAKE_BUILD_TYPE "Release")
    file(GLOB_RECURSE benchmark_src
        benchmark/*.cpp
        benchmark/*.h
        benchmark/*.hpp
    )
    add_executable(benchmark ${benchmark_src} ${json_headers})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${json_headers})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${benchmark_src})

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|EM64T|x86_64")
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
            target_compile_options(benchmark PRIVATE "/arch:AVX2")
        elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "GNU|Clang")
            target_compile_options(benchmark PRIVATE "-mavx2")
        endif()
    endif()
endif()
