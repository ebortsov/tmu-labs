cmake_minimum_required(VERSION 3.13)
project(sheet06-scope)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_executable(tests
        Stopwatch.cpp
        Scope.cpp
        tests/test_scope.cpp
        tests/test_traits.cpp)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)