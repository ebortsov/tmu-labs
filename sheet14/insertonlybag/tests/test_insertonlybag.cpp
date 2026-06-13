#include "../insertonlybag.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <iterator>
#include <thread>
//---------------------------------------------------------------------------
using namespace std;
using namespace bag;
//---------------------------------------------------------------------------
TEST(InsertOnlyBag, SizeAndTraits)
{
    ASSERT_EQ(sizeof(InsertOnlyBag<int>::Node), 16);
    ASSERT_EQ(sizeof(InsertOnlyBag<int>), 8);
    ASSERT_TRUE(std::input_iterator<typename InsertOnlyBag<int>::Iterator>);
}

TEST(InsertOnlyBag, InsertSingle)
{
    InsertOnlyBag<int> bag;
    ASSERT_EQ(bag.begin(), bag.end());
    bag.insert(1);
    ASSERT_NE(bag.begin(), bag.end());
    ASSERT_EQ(*bag.begin(), 1);
    ASSERT_EQ(++bag.begin(), bag.end());
}

class X
{
public:
    X(int, float, double) {}
    X(const X&) = delete;
    X(X&&) = delete;
    X& operator=(const X&) = delete;
    X& operator=(X&&) = delete;
};

TEST(InsertOnlyBag, EmplaceSingle)
{
    InsertOnlyBag<X> bag;
    ASSERT_EQ(bag.begin(), bag.end());
    bag.emplace(1, 0.2f, 0.3);
    ASSERT_NE(bag.begin(), bag.end());
    ASSERT_EQ(++bag.begin(), bag.end());
}

TEST(InsertOnlyBag, InsertMultiple)
{
    InsertOnlyBag<int> bag;
    for (int i = 0; i < 10; i++)
        bag.insert(i);
    std::vector<int> values(bag.begin(), bag.end());
    std::sort(values.begin(), values.end());
    for (int i = 0; i < 10; i++)
        ASSERT_EQ(values[i], i);
}

TEST(InsertOnlyBag, ParallelInsert1)
{
    InsertOnlyBag<int> bag;

    vector<thread> threads;
    for (int value = 0; value < 6; ++value)
        threads.emplace_back([&bag, value = value]() {
            for (int i = 0; i < 1000; i++) {
                bag.insert(value * 1000 + i);
            }
        });
    for (auto& thread : threads)
        thread.join();

    std::vector<int> values(bag.begin(), bag.end());
    std::sort(values.begin(), values.end());
    for (int i = 0; i < 6 * 1000; i++)
        ASSERT_EQ(values[i], i);
}

TEST(InsertOnlyBag, ParallelInsert2)
{
    InsertOnlyBag<int> bag;

    vector<thread> threads;
    for (int value = 0; value < 6; ++value)
        threads.emplace_back([&bag, value = value]() {
            for (int i = 0; i < 1000; i++) {
                bag.insert(value);
            }
        });
    for (auto& thread : threads)
        thread.join();

    std::vector<int> values(bag.begin(), bag.end());
    std::sort(values.begin(), values.end());
    for (int i = 0; i < 6 * 1000; i++)
        ASSERT_EQ(values[i], i / 1000);
}

TEST(InsertOnlyBag, ParallelInsert3)
{
    InsertOnlyBag<int> bag;

    vector<thread> threads;
    for (int value = 0; value < 4; ++value)
        threads.emplace_back([&bag, value = value]() {
            for (int i = 0; i < 1000; i++) {
                bag.insert(value);
            }
        });
    for (int value = 0; value < 2; ++value)
        threads.emplace_back([&bag]() {
            int x;
            int* ptr = &x;
            for (int i = 0; i < 1000; i++)
                for (auto& elem : bag)
                    *ptr = elem;
        });
    for (auto& thread : threads)
        thread.join();

    std::vector<int> values(bag.begin(), bag.end());
    std::sort(values.begin(), values.end());
    for (int i = 0; i < 4 * 1000; i++)
        ASSERT_EQ(values[i], i / 1000);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
