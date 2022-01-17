cmake_minimum_required(VERSION 3.0)
project(meojson)

aux_source_directory(src SRC)
include_directories(./include)

option(BUILD_SAMPLE "Build sample in sample/sample.cpp" ON)

# add_compile_options(-Wall -std=c++17 -O2)
find_package(Threads)
# set(CMAKE_BUILD_TYPE Release) # 自带-O2选项
set(CMAKE_CXX_STANDARD 17)    # std=c++17

# add_library(meojson STATIC ${SRC})

if (BUILD_SAMPLE)
  set(SAMPLE_SRC sample/sample.cpp)
  add_executable(sample.out ${SAMPLE_SRC})
endif()

# target_link_libraries(sample.out meojson ${CMAKE_THREAD_LIBS_INIT})