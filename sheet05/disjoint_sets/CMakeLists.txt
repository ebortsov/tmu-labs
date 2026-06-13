cmake_minimum_required(VERSION 3.13)
project(sheet05-disjoint-sets)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_executable(tests disjoint_sets.cpp tests/test_disjoint_sets.cpp)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)