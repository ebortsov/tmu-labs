cmake_minimum_required(VERSION 3.13)
project(sheet12-mixins)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_executable(tests mixins.cpp tests/test_main.cpp tests/test_base64.cpp tests/test_usage.cpp)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined)
target_link_options(tests PRIVATE -fsanitize=address,undefined)