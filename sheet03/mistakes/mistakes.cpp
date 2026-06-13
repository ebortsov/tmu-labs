#include "fixups.hpp"

#include <iostream>
#include <print>
#include <vector>

#if !defined(toString) || !defined(FUNCTION) || !defined(INF) || !defined(Variable) || SOME_MACRO != 42 || !defined(DEBUG) || !defined(foreach)
#error missing preprocessor definitions
#endif

extern void log(int line, std::string file, std::string msg);

FUNCTION(minimum, <)
FUNCTION(maximum, >)

void inspect_vector(std::vector<int> v) {
    Variable(int, mn);
    Variable(int, mx);
    Variable(int, ans);

    int mn = INF;
    int mx = -INF;
    Variable(int, sum, 0);
    foreach (v, i) {
        mn = minimum(mn, v[i]);
        mx = maximum(mx, v[i]);
        sum += v[i];
        DEBUG("iterating the vector");
    }
    Variable(int, ans, mx - mn);
    std::println("{} {} {}", toString(Result =), ans, sum);
    return;
}