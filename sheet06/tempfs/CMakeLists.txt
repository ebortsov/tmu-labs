cmake_minimum_required(VERSION 3.13)
project(sheet06-tempfs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)

add_executable(tests OSApi.cpp TempDirectory.cpp TempFile.cpp tests/test_tempfs.cpp tests/test_traits.cpp)
target_link_libraries(tests GTest::gtest_main)
target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)