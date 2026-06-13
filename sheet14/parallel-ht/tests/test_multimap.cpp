#include "../parallel-multimap.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <unordered_set>
//---------------------------------------------------------------------------
using namespace std;
using namespace hashtable;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
struct PairHash
{
    size_t operator()(const pair<uint32_t, uint32_t>& pair) const
    {
        return hash<uint32_t>()(pair.first) ^ hash<uint32_t>()(pair.second);
    }
};
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, InsertFindUnique)
{
    ParallelMultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 4; ++value)
            map.insert({key, value});

    for (uint32_t key = 0; key < 32; ++key) {
        unordered_multiset<uint32_t> values;

        auto it = map.findFirst(key);

        while (it != map.end()) {
            values.insert(it->second);
            it = map.findNext(it);
        }

        for (uint32_t value = 0; value < 4; ++value)
            ASSERT_TRUE(values.count(value) == 1);
        ASSERT_EQ(values.size(), 4);
    }
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, InsertFindNonUnique)
{
    ParallelMultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t i = 0; i < 4; ++i)
            map.insert({key, key});

    for (uint32_t key = 0; key < 32; ++key) {
        unordered_multiset<uint32_t> values;

        auto it = map.findFirst(key);

        while (it != map.end()) {
            values.insert(it->second);
            it = map.findNext(it);
        }

        ASSERT_TRUE(values.count(key) == 4);
        ASSERT_EQ(values.size(), 4);
    }
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, InsertIterateUnique)
{
    ParallelMultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 4; ++value)
            map.insert({key, value});

    unordered_multiset<pair<uint32_t, uint32_t>, PairHash> reference;
    for (auto& it : map)
        reference.insert({it.first, it.second});

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 4; ++value)
            ASSERT_EQ(reference.count({key, value}), 1);
    ASSERT_EQ(reference.size(), 32 * 4);
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, InsertIterateNonUnique)
{
    ParallelMultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t i = 0; i < 4; ++i)
            map.insert({key, key});

    unordered_multiset<pair<uint32_t, uint32_t>, PairHash> reference;
    for (auto& it : map)
        reference.insert({it.first, it.second});

    for (uint32_t key = 0; key < 32; ++key)
        ASSERT_EQ(reference.count({key, key}), 4);
    ASSERT_EQ(reference.size(), 32 * 4);
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, ParallelInsertFindUnique)
{
    ParallelMultiMap map(32);

    vector<thread> threads;
    for (uint32_t value = 0; value < 4; ++value)
        threads.emplace_back([&map, value]() {
            for (uint32_t i = 0; i < 16; ++i)
                for (uint32_t key = 0; key < 32; ++key)
                    map.insert({key, 16 * value + i}); });

    for (auto& thread : threads)
        thread.join();

    for (uint32_t key = 0; key < 32; ++key) {
        unordered_multiset<uint32_t> values;

        auto it = map.findFirst(key);

        while (it != map.end()) {
            values.insert(it->second);
            it = map.findNext(it);
        }

        for (uint32_t value = 0; value < 64; ++value)
            ASSERT_TRUE(values.count(value) == 1);
        ASSERT_EQ(values.size(), 64);
    }
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, ParallelInsertFindNonUnique)
{
    ParallelMultiMap map(32);

    vector<thread> threads;
    for (uint32_t value = 0; value < 4; ++value)
        threads.emplace_back([&map]() {
            for (uint32_t i = 0; i < 16; ++i)
                for (uint32_t key = 0; key < 32; ++key)
                    map.insert({key, key}); });

    for (auto& thread : threads)
        thread.join();

    for (uint32_t key = 0; key < 32; ++key) {
        unordered_multiset<uint32_t> values;

        auto it = map.findFirst(key);

        while (it != map.end()) {
            values.insert(it->second);
            it = map.findNext(it);
        }

        ASSERT_TRUE(values.count(key) == 64);
        ASSERT_EQ(values.size(), 64);
    }
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, ParallelInsertIterateUnique)
{
    ParallelMultiMap map(32);

    vector<thread> threads;
    for (uint32_t value = 0; value < 4; ++value)
        threads.emplace_back([&map, value]() {
            for (uint32_t i = 0; i < 16; ++i)
                for (uint32_t key = 0; key < 32; ++key)
                    map.insert({key, 16 * value + i}); });

    for (auto& thread : threads)
        thread.join();

    unordered_multiset<pair<uint32_t, uint32_t>, PairHash> reference;
    for (auto& it : map)
        reference.insert({it.first, it.second});

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 64; ++value)
            ASSERT_EQ(reference.count({key, value}), 1);
    ASSERT_EQ(reference.size(), 32 * 64);
}
//---------------------------------------------------------------------------
TEST(ParallelMultiMap, ParallelInsertIterateNonUnique)
{
    ParallelMultiMap map(32);

    vector<thread> threads;
    for (uint32_t value = 0; value < 4; ++value)
        threads.emplace_back([&map]() {
            for (uint32_t i = 0; i < 16; ++i)
                for (uint32_t key = 0; key < 32; ++key)
                    map.insert({key, key}); });

    for (auto& thread : threads)
        thread.join();

    unordered_multiset<pair<uint32_t, uint32_t>, PairHash> reference;
    for (auto& it : map)
        reference.insert({it.first, it.second});

    for (uint32_t key = 0; key < 32; ++key)
        ASSERT_EQ(reference.count({key, key}), 64);
    ASSERT_EQ(reference.size(), 32 * 64);
}
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
