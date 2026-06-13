#include "../stockpiles.hpp"
#include <random>
#include <gtest/gtest.h>

void run(std::vector<unsigned> &stockpile_values,
         std::vector<unsigned> &requests,
         unsigned last_request,
         std::vector<unsigned> &expected_ids) {
    std::vector<unsigned> expected_values_at_end = stockpile_values;
    for (unsigned i = 0; i < expected_ids.size(); i += 1) { expected_values_at_end[expected_ids[i]] -= requests[i]; }
    std::vector<stockpiles::Stockpile> stockpiles;
    stockpiles.reserve(stockpile_values.size());
    for (unsigned i = 0; i < stockpile_values.size(); i += 1) {
        stockpiles.push_back(stockpiles::Stockpile{.id = i, .bottles = &stockpile_values[i]});
    }

    stockpiles::init(stockpiles.data(), stockpiles.size());

    for (unsigned i = 0; i < expected_ids.size(); i += 1) {
        stockpiles::Stockpile which;
        unsigned expected_value = stockpile_values[expected_ids[i]] - requests[i];
        bool result = stockpiles::serve(stockpiles.data(), stockpiles.size(), requests[i], which);
        ASSERT_TRUE(result);
        ASSERT_EQ(expected_ids[i], which.id);
        ASSERT_EQ(&stockpile_values[expected_ids[i]], which.bottles);
        ASSERT_EQ(stockpile_values[expected_ids[i]], expected_value);
    }

    stockpiles::Stockpile which;
    bool result = stockpiles::serve(stockpiles.data(), stockpiles.size(), last_request, which);
    ASSERT_FALSE(result);

    ASSERT_EQ(expected_values_at_end, stockpile_values);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Empty) {
    std::vector<unsigned> stockpile_values = {};
    std::vector<unsigned> requests = {};
    std::vector<unsigned> expected_ops = {};

    run(stockpile_values, requests, 1, expected_ops);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Small01) {
    std::vector<unsigned> stockpile_values = {3, 5, 7};
    std::vector<unsigned> requests = {3, 4, 4, 2};
    std::vector<unsigned> expected_ops = {2, 1, 2, 0};

    run(stockpile_values, requests, 2, expected_ops);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Small02) {
    std::vector<unsigned> stockpile_values = {2, 2, 2};
    std::vector<unsigned> requests = {1, 1, 1, 1, 1, 1};
    std::vector<unsigned> expected_ops = {0, 1, 2, 0, 1, 2};

    run(stockpile_values, requests, 1, expected_ops);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Small03) {
    std::vector<unsigned> stockpile_values = {2, 2, 2};
    std::vector<unsigned> requests = {1, 1, 1, 1, 1};
    std::vector<unsigned> expected_ops = {0, 1, 2, 0, 1};

    run(stockpile_values, requests, 2, expected_ops);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Small04) {
    std::vector<unsigned> stockpile_values = {1, 2, 3, 1, 2, 3, 1, 2, 3, 4};
    std::vector<unsigned> requests = {3, 3, 2, 2, 1, 1, 2};
    std::vector<unsigned> expected_ops = {9, 2, 5, 8, 1, 4, 7};

    run(stockpile_values, requests, 2, expected_ops);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Small05) {
    std::vector<unsigned> stockpile_values = {7, 13, 7, 2, 2, 4, 14, 5, 2, 7, 4};
    std::vector<unsigned> requests = {7, 3, 2, 2, 2, 6, 7, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1};
    std::vector<unsigned> expected_ops = {6, 1, 1, 1, 0, 2, 6, 9, 1, 0, 7, 5, 10, 9, 1, 3, 4, 8, 0, 2};

    run(stockpile_values, requests, 2, expected_ops);
}

//---------------------------------------------------------------------------
TEST(TestStockpiles, Medium01) {
    const unsigned SIZE = 100;

    std::vector<unsigned> stockpile_values(SIZE);
    std::vector<unsigned> requests(SIZE);
    unsigned val = 2 * SIZE;
    unsigned req = SIZE;
    for (unsigned i = 0; i < SIZE; i += 1) {
        stockpile_values[i] = val;
        requests[i] = req;
        val -= 2;
        req -= 1;
    }
    std::vector<unsigned> expected_ops = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
                                          17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
                                          34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 0,
                                          50, 1,  2,  51, 3,  4,  52, 5,  6,  53, 7,  8,  54, 9,  10, 55, 11,
                                          12, 56, 13, 14, 57, 15, 16, 58, 17, 18, 59, 19, 20, 60, 21, 22, 61,
                                          23, 24, 62, 25, 26, 63, 27, 28, 64, 29, 30, 65, 31, 32, 66};

    run(stockpile_values, requests, 2 * SIZE, expected_ops);
}
//---------------------------------------------------------------------------
void generate_and_run(unsigned values_size, unsigned requests_size) {
    std::vector<unsigned> stockpile_values(values_size);
    std::vector<unsigned> requests(requests_size);
    std::vector<unsigned> expected_ops(requests_size);
    for (unsigned i = 0; i < values_size; i += 1) { stockpile_values[i] = requests_size - i; }
    std::fill(requests.begin(), requests.end(), 1);
    unsigned current = 0;
    unsigned upto = 1;
    for (unsigned i = 0; i < requests_size; i += 1) {
        expected_ops[i] = current;
        current += 1;
        if (current == upto) {
            current = 0;
            upto += 1;
        }
    }

    run(stockpile_values, requests, requests_size, expected_ops);
}
//---------------------------------------------------------------------------
TEST(TestStockpiles, Medium02) { generate_and_run(100, 1'000); }
//---------------------------------------------------------------------------
TEST(TestStockpiles, Medium03) { generate_and_run(1'000, 1'000); }
//---------------------------------------------------------------------------
TEST(TestStockpiles, Medium04) { generate_and_run(1'000, 2'500); }
//---------------------------------------------------------------------------
TEST(TestStockpiles, Large01) { generate_and_run(10'000, 100'000); }
//---------------------------------------------------------------------------
TEST(TestStockpiles, Large02) { generate_and_run(100'000, 100'000); }
//---------------------------------------------------------------------------
TEST(TestStockpiles, Large03) { generate_and_run(100'000, 1'000'000); }
//---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}