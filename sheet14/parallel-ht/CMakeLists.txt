cmake_minimum_required(VERSION 3.13)
project(sheet14)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_executable(tests multimap.cpp parallel-multimap.cpp tests/test_multimap.cpp)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=thread)
target_link_options(tests PRIVATE -fsanitize=thread)