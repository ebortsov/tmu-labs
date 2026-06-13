#include "../sport.hpp"
#include <random>
#include <gtest/gtest.h>

void run(std::vector<unsigned> &points, std::vector<unsigned> &times, std::vector<unsigned> &expected_order) {
    std::vector<unsigned> expected_points = points;
    std::vector<unsigned> expected_times = times;
    std::vector<const unsigned *> points_ptrs;
    std::vector<const unsigned *> times_ptrs;
    std::vector<const unsigned *> expected_points_ptrs;
    std::vector<const unsigned *> expected_times_ptrs;
    points_ptrs.reserve(points.size());
    times_ptrs.reserve(times.size());
    expected_points_ptrs.reserve(points.size());
    expected_times_ptrs.reserve(times.size());
    for (unsigned i = 0; i < points.size(); i += 1) {
        points_ptrs.push_back(&points[i]);
        times_ptrs.push_back(&times[i]);
        expected_points_ptrs.push_back(&points[expected_order[i]]);
        expected_times_ptrs.push_back(&times[expected_order[i]]);
    }
    sport::sort(points_ptrs.data(), times_ptrs.data(), points_ptrs.size());

    ASSERT_EQ(expected_points, points);
    ASSERT_EQ(expected_times, times);
    ASSERT_EQ(expected_points_ptrs, points_ptrs);
    ASSERT_EQ(expected_times_ptrs, times_ptrs);
}
//---------------------------------------------------------------------------
TEST(TestSport, SortEmpty) {
    std::vector<unsigned> points = {};
    std::vector<unsigned> times = {};
    std::vector<unsigned> expected_order = {};

    run(points, times, expected_order);
}
//---------------------------------------------------------------------------
TEST(TestSport, Sort1) {
    std::vector<unsigned> points = {1, 2, 3, 4};
    std::vector<unsigned> times = {1, 1, 1, 1};
    std::vector<unsigned> expected_order = {3, 2, 1, 0};

    run(points, times, expected_order);
}
//---------------------------------------------------------------------------
TEST(TestSport, Sort2) {
    std::vector<unsigned> points = {7, 11, 3, 2, 3, 3, 9, 9, 7};
    std::vector<unsigned> times = {100, 5, 4, 5, 5, 3, 8, 6, 15};
    std::vector<unsigned> expected_order = {1, 7, 6, 8, 0, 5, 2, 4, 3};

    run(points, times, expected_order);
}
//---------------------------------------------------------------------------
TEST(TestSport, Sort3) {
    std::vector<unsigned> points = {1, 2, 3, 1, 2, 3, 1, 2, 3, 4};
    std::vector<unsigned> times = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2};
    std::vector<unsigned> expected_order = {9, 8, 5, 2, 4, 1, 7, 0, 6, 3};

    run(points, times, expected_order);
}
//---------------------------------------------------------------------------
TEST(TestSport, Sort4) {
    const unsigned SIZE = 100'000;
    std::vector<unsigned> points(SIZE);
    std::vector<unsigned> times(SIZE);
    std::vector<unsigned> expected_order(SIZE);
    std::iota(expected_order.begin(), expected_order.end(), 0);
    std::shuffle(expected_order.begin(), expected_order.end(), std::mt19937(2137));

    unsigned p = SIZE;
    unsigned t = 1;

    for (unsigned i : expected_order) {
        points[i] = p;
        times[i] = t;
        if (t == 5) {
            p -= 1;
            t = 1;
        } else {
            t += 1;
        }
    }

    run(points, times, expected_order);
}
//---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
