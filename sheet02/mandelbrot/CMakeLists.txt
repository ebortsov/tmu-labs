cmake_minimum_required(VERSION 3.20)
project(sheet02)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest CONFIG REQUIRED)

add_executable(tests mandelbrot.cpp tests/test_mandelbrot.cpp)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror)