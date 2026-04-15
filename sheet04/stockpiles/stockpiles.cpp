#include "stockpiles.hpp"

namespace stockpiles {
namespace {
// return true if s1 should be served earlier than s2
bool serve_first(Stockpile &s1, Stockpile &s2) {
    return *s1.bottles > *s2.bottles || (*s1.bottles == *s2.bottles && s1.id < s2.id);
}

void swap_stockpiles(Stockpile &s1, Stockpile &s2) {
    Stockpile tmp = s1;
    s1 = s2;
    s2 = tmp;
}
} // namespace
void init(Stockpile *stock_start, unsigned stock_len) {
    if (stock_len == 0) { return; }

    for (unsigned i = 1; i < stock_len; i++) {
        unsigned node = i;
        while (node > 0) {
            unsigned parent = (node - 1) / 2;
            if (serve_first(stock_start[node], stock_start[parent])) {
                swap_stockpiles(stock_start[node], stock_start[parent]);
                node = parent;
            } else {
                break;
            }
        }
    }
}
bool serve(Stockpile *stock_start, unsigned stock_len, unsigned amount, Stockpile &which) {
    if (stock_len == 0 || *stock_start[0].bottles < amount) { return false; }
    which = stock_start[0];
    *stock_start[0].bottles -= amount;
    unsigned node = 0;
    while (node * 2 + 1 < stock_len) {
        unsigned left = node * 2 + 1;
        unsigned right = node * 2 + 2;
        unsigned largest_node = left;
        if (node * 2 + 2 < stock_len && serve_first(stock_start[right], stock_start[left])) { largest_node = right; }
        if (serve_first(stock_start[largest_node], stock_start[node])) {
            swap_stockpiles(stock_start[node], stock_start[largest_node]);
            node = largest_node;
        } else {
            break;
        }
    }
    return true;
}
} // namespace stockpiles