cmake_minimum_required(VERSION 3.14)

project(testMeojson)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/../install)

find_package(meojson CONFIG REQUIRED)

add_executable(testMeojson main.cpp)
target_link_libraries(testMeojson PRIVATE meojson::meojson)
