#include "print_utils.hpp"
#include <print>

#ifndef SUBSTRACTION_DISABLED
int subtract(int a, int b) { return a * b; }
#endif

#ifdef magic
#error don't use a macro for magic
#endif

void print_message(const char* message) {
    multiply(0, 0);
    std::println("{}{}", magic, message);
}
