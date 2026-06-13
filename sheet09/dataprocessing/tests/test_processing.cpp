#include "../processing.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <random>

TEST(Processing, Empty)
{
    DataChunk chunk({}, 0);
    chunk.columns.emplace_back(std::make_unique<int64_t[]>(0));
    testing::internal::CaptureStdout();

    process(chunk, chunk, 0, 0, 0, 10);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST(Processing, SingleColumn)
{
    DataChunk chunk({}, 4);
    auto& col = chunk.columns.emplace_back(std::make_unique<int64_t[]>(4));
    col[0] = 0;
    col[1] = 1;
    col[2] = 1;
    col[3] = 2;
    testing::internal::CaptureStdout();
    process(chunk, chunk, 0, 0, 0, 1);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), std::format("0: 1@{}\n", reinterpret_cast<uintptr_t>(col.get())));
}

TEST(Processing, SingleColumnTwoValues)
{
    DataChunk chunk({}, 100);
    auto& col = chunk.columns.emplace_back(std::make_unique<int64_t[]>(100));
    std::fill(col.get(), col.get() + 50, 1);
    std::fill(col.get() + 50, col.get() + 100, 2);
    testing::internal::CaptureStdout();
    process(chunk, chunk, 0, 0, 0, 10);
    ASSERT_EQ(testing::internal::GetCapturedStdout(),
        std::format("1: 1@{}\n2: 1@{}\n", reinterpret_cast<uintptr_t>(col.get()), reinterpret_cast<uintptr_t>(col.get() + 50)));
}

TEST(Processing, DifferentSides)
{
    DataChunk chunkR({}, 1);
    {
        auto& col = chunkR.columns.emplace_back(std::make_unique<int64_t[]>(1));
        col[0] = 1;
    }
    DataChunk chunk({}, 4);
    auto& col = chunk.columns.emplace_back(std::make_unique<int64_t[]>(4));
    col[0] = 0;
    col[1] = 1;
    col[2] = 1;
    col[3] = 2;

    testing::internal::CaptureStdout();
    process(chunk, chunkR, 0, 0, 0, 1);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), std::format("1: 1@{}\n", reinterpret_cast<uintptr_t>(col.get() + 1)));
}

TEST(Processing, EmptyRhs)
{
    DataChunk chunk1({}, 100);
    auto& col = chunk1.columns.emplace_back(std::make_unique<int64_t[]>(100));
    std::fill(col.get(), col.get() + 50, 1);
    std::fill(col.get() + 50, col.get() + 100, 2);
    DataChunk chunk2({}, 0);
    chunk2.columns.emplace_back(std::make_unique<int64_t[]>(0));
    testing::internal::CaptureStdout();

    process(chunk1, chunk2, 0, 0, 0, 10);
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "");
}

TEST(Processing, MultipleColumns1)
{
    DataChunk chunk1({}, 10);
    {
        auto& col = chunk1.columns.emplace_back(std::make_unique<int64_t[]>(10));
        std::fill(col.get(), col.get() + 5, 1);
        std::fill(col.get() + 5, col.get() + 10, 2);
    }
    {
        auto& col = chunk1.columns.emplace_back(std::make_unique<int64_t[]>(10));
        std::fill(col.get(), col.get() + 1, 42);
        std::fill(col.get() + 1, col.get() + 10, 43);
    }

    auto& col = chunk1.columns.emplace_back(std::make_unique<int64_t[]>(10));
    std::fill(col.get(), col.get() + 1, 200);
    std::fill(col.get() + 1, col.get() + 10, 100);

    testing::internal::CaptureStdout();
    process(chunk1, chunk1, 0, 1, 2, 1);
    ASSERT_EQ(testing::internal::GetCapturedStdout(),
        std::format("200: 1@{}\n", reinterpret_cast<uintptr_t>(col.get())));
}

TEST(Processing, Large1)
{
    constexpr size_t size = 10'000;
    DataChunk chunk1({}, size);
    DataChunk chunk2({}, size / 2);

    std::mt19937 engine(2137);
    std::uniform_int_distribution<int64_t> dist(0, 20);
    auto gen = [&]() { return dist(engine); };
    for (unsigned i = 0; i < 20; i++) {
        auto& col1 = chunk1.columns.emplace_back(std::make_unique<int64_t[]>(size));
        auto& col2 = chunk2.columns.emplace_back(std::make_unique<int64_t[]>(size / 2));
        std::generate(col1.get(), col1.get() + size, gen);
        std::generate(col2.get(), col2.get() + size / 2, gen);
    }
    testing::internal::CaptureStdout();
    process(chunk1, chunk2, 0, 1, 2, 1);
    const auto output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.starts_with("18: 3@")) << output;
}

TEST(Processing, Large2)
{
    constexpr size_t size = 1'000;
    DataChunk chunk1({}, size);
    DataChunk chunk2({}, size / 2);

    std::mt19937 engine(2137);
    std::uniform_int_distribution<int64_t> dist(4, 5);
    auto gen = [&]() { return dist(engine); };
    for (unsigned i = 0; i < 20; i++) {
        auto& col1 = chunk1.columns.emplace_back(std::make_unique<int64_t[]>(size));
        auto& col2 = chunk2.columns.emplace_back(std::make_unique<int64_t[]>(size / 2));
        std::generate(col1.get(), col1.get() + size, gen);
        std::generate(col2.get(), col2.get() + size / 2, gen);
    }
    testing::internal::CaptureStdout();
    process(chunk1, chunk2, 0, 1, 2, 1);
    const auto output = testing::internal::GetCapturedStdout();
    ASSERT_TRUE(output.starts_with("5: 2@")) << output;
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
