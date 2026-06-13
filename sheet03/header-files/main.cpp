#include "math_operations.hpp"
#include "print_utils.hpp"
#include <print>

int magic = 0;

int entry() {
    int a = 10, b = 5;

    std::println("Addition: {}", add(a, b));
    std::println("Subtraction: {}", subtract(a, b));
    std::println("Multiplication: {}", multiply(a, b));
    std::println("Division: {}", divide(a, b));
#ifdef get42
#error don't use a macro for get42
#endif
    std::println("42: {}", get42());

    print_message("All operations are successful!");

    return 0;
}
