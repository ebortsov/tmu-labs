#include "sport.hpp"
#include <cstdint>

namespace sport {
using std::size_t;

namespace {
// Return true is player 1 is weaker than player 2
bool is_weaker(unsigned points1, unsigned time1, unsigned points2, unsigned time2) {
    return points1 < points2 || (points1 == points2 && time1 > time2);
}

void swap_pointers(void *p1, void *p2) {

}
} // namespace

void sort(const unsigned **points, const unsigned **times, unsigned sz) {
    for (size_t i = 1; i < sz; i++) {
        size_t node = i;
        while (node > 0) {
            size_t parent = (node - 1) / 2;
            if (is_weaker(*points[node], *times[node], *points[parent], *times[parent])) {
                swap_pointers(points[node], points[parent]);
                swap_pointers(times[node], times[parent]);
                node = parent;
            } else {
                break;
            }
        }
    }

    for (size_t i = sz - 1; i > 0; i--) {
        swap_pointers(points[0], points[i]);
        swap_pointers(times[0], times[i]);
        
        size_t node = 0;
        size_t left{};
        while ((left = node * 2 + 1) < i) {
            size_t right = node * 2 + 2;
            size_t weakest_node = left;
            if (right < i && is_weaker(*points[right], *times[right], *points[left], *times[left])) {
                weakest_node = right;
            }
            if (is_weaker(*points[weakest_node], *times[weakest_node], *points[node], *times[node])) {
                swap_pointers(points[node], points[weakest_node]);
                swap_pointers(times[node], times[weakest_node]);
            }
            swap_pointers(points[node], points[weakest_node]);
            swap_pointers(times[node], times[weakest_node]);
            node = weakest_node;
        }
    }
}
} // namespace sport