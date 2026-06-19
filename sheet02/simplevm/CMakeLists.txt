cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(sheet02)

find_package(GTest CONFIG REQUIRED)

add_executable(tests simplevm.cpp tests/test_simplevm.cpp)
target_compile_options(tests PRIVATE -g)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined)
target_link_options(tests PRIVATE -fsanitize=undefined)