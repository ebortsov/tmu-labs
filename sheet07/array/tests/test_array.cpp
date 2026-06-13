#include "../array.hpp"
#include <gtest/gtest.h>

namespace {
class Empty
{};
class Elem
{
public:
    Elem() = default;
    Elem(int v) : x(v) {}
    Elem(Elem&& other) : x(other.x)
    {
        other.x = -1;
    }

    Elem& operator=(Elem&& other)
    {
        x = other.x;
        other.x = -1;
        return *this;
    }

    Elem& operator=(const Elem& other)
    {
        x = other.x;
        return *this;
    }

    bool operator==(const Elem& rhs) const = default;

    int x;
};
}

namespace otl {

TEST(OTL_ARRAY, traits)
{
    ASSERT_FALSE((std::is_copy_assignable_v<array<char, 1>>));
    ASSERT_FALSE((std::is_move_assignable_v<array<char, 1>>));
    ASSERT_TRUE((std::is_same_v<typename array<char, 1>::value_type, char>));
    ASSERT_TRUE((std::is_same_v<typename array<char, 1>::reference, char&>));
    ASSERT_TRUE((std::is_same_v<typename array<char, 1>::const_reference, const char&>));
    ASSERT_EQ(sizeof(array<char, 1>::size_type), 8);

    ASSERT_FALSE((std::is_copy_assignable_v<array<float, 1>>));
    ASSERT_FALSE((std::is_move_assignable_v<array<float, 1>>));
    ASSERT_TRUE((std::is_same_v<typename array<float, 1>::value_type, float>));
    ASSERT_TRUE((std::is_same_v<typename array<float, 1>::reference, float&>));
    ASSERT_TRUE((std::is_same_v<typename array<float, 1>::const_reference, const float&>));
    ASSERT_EQ(sizeof(array<float, 1>::size_type), 8);

    ASSERT_FALSE((std::is_copy_assignable_v<array<Elem, 1>>));
    ASSERT_FALSE((std::is_move_assignable_v<array<Elem, 1>>));
    ASSERT_TRUE((std::is_same_v<typename array<Elem, 1>::value_type, Elem>));
    ASSERT_TRUE((std::is_same_v<typename array<Elem, 1>::reference, Elem&>));
    ASSERT_TRUE((std::is_same_v<typename array<Elem, 1>::const_reference, const Elem&>));
    ASSERT_EQ(sizeof(array<Elem, 1>::size_type), 8);

    ASSERT_FALSE((std::is_copy_assignable_v<array<bool, 1>>));
    ASSERT_FALSE((std::is_move_assignable_v<array<bool, 1>>));
    ASSERT_TRUE((std::is_same_v<typename array<bool, 1>::value_type, bool>));
    ASSERT_EQ(sizeof(array<bool, 1>::size_type), 8);
}

TEST(OTL_ARRAY, Size)
{
    ASSERT_EQ((array<char, 1>{}.size()), 1);
    ASSERT_EQ((array<char, 2>{}.size()), 2);
    ASSERT_EQ((array<int32_t, 2>{}.size()), 2);
    ASSERT_EQ((array<int64_t, 10>{}.size()), 10);

    ASSERT_EQ((array<Empty, 10>{}.size()), 10);
    ASSERT_EQ((array<Elem, 10>{}.size()), 10);

    ASSERT_EQ((array<bool, 1>{}.size()), 1);
    ASSERT_EQ((array<bool, 8>{}.size()), 8);
    ASSERT_EQ((array<bool, 32>{}.size()), 32);
}

TEST(OTL_ARRAY, ClassSize)
{
    ASSERT_EQ(sizeof(array<char, 1>), 1);
    ASSERT_EQ(sizeof(array<char, 2>), 2);
    ASSERT_EQ(sizeof(array<int32_t, 2>), 8);
    ASSERT_EQ(sizeof(array<int64_t, 10>), 80);

    ASSERT_EQ(sizeof(array<Empty, 10>), 10);
    ASSERT_EQ(sizeof(array<Elem, 10>), 40);

    ASSERT_EQ(sizeof(array<bool, 1>), 1);
    ASSERT_EQ(sizeof(array<bool, 8>), 1);
    ASSERT_EQ(sizeof(array<bool, 32>), 4);
}

TEST(OTL_ARRAY, ConstructFromValue)
{
    {
        const array<char, 5> arr(42);
        ASSERT_EQ(arr[0], 42);
        ASSERT_EQ(arr[4], 42);
    }
    {
        const array<bool, 5> arr(true);
        ASSERT_EQ(arr[0], true);
        ASSERT_EQ(arr[4], true);
    }
}

TEST(OTL_ARRAY, CopyFromOther)
{
    array<Elem, 5> init(42);
    const array<Elem, 10> arr(init);
    ASSERT_EQ(init[0].x, 42);
    ASSERT_EQ(init[4].x, 42);
    ASSERT_EQ(arr[0].x, 42);
    ASSERT_EQ(arr[4].x, 42);
    ASSERT_EQ(arr[5].x, 0);
    ASSERT_EQ(arr[9].x, 0);
}

TEST(OTL_ARRAY, MoveFromOther)
{
    array<Elem, 5> init(42);
    const array<Elem, 10> arr(std::move(init));
    ASSERT_EQ(init[0].x, -1);
    ASSERT_EQ(init[4].x, -1);
    ASSERT_EQ(arr[0].x, 42);
    ASSERT_EQ(arr[4].x, 42);
    ASSERT_EQ(arr[5].x, 0);
    ASSERT_EQ(arr[9].x, 0);
}

TEST(OTL_ARRAY, AssignElements)
{
    {
        array<Elem, 5> arr(42);
        arr[3] = Elem{0};
        Elem elem{100};
        arr[2] = std::move(elem);
        ASSERT_EQ(arr[0].x, 42);
        ASSERT_EQ(arr[1].x, 42);
        ASSERT_EQ(arr[2].x, 100);
        ASSERT_EQ(arr[3].x, 0);
        ASSERT_EQ(arr[4].x, 42);
    }
    {
        array<bool, 10> arr(false);
        arr.set(0);
        arr.set(2);
        arr.unset(2);
        ASSERT_EQ(arr[0], true);
        ASSERT_EQ(arr[2], false);
        ASSERT_EQ(arr[9], false);
    }
}

#define EXPECT_THROW_WITH_MESSAGE(statement, txt)     \
    try {                                             \
        statement;                                    \
        FAIL() << "Expected exception!";              \
    } catch (std::runtime_error & msg) {              \
        ASSERT_EQ(std::string_view(msg.what()), txt); \
    } catch (...) {                                   \
        FAIL() << "Expected const char*!";            \
    }

TEST(OTL_ARRAY, InvalidIndex)
{
    {
        const array<Elem, 5> arr(42);
        EXPECT_THROW_WITH_MESSAGE(arr[5], "invalid access");
        EXPECT_THROW_WITH_MESSAGE(arr[100], "invalid access");
    }
    {
        array<bool, 5> arr;
        EXPECT_THROW_WITH_MESSAGE(arr[5], "invalid access");
        EXPECT_THROW_WITH_MESSAGE(arr[100], "invalid access");
    }
}

TEST(OTL_ARRAY, CopyTooLarge)
{
    {
        EXPECT_THROW_WITH_MESSAGE((array<bool, 3>{array<bool, 31>{}}), "too large");
        EXPECT_THROW_WITH_MESSAGE((array<Elem, 1>{array<Elem, 23>{}}), "too large");
    }
    {
        const array<bool, 123> a{};
        const array<Elem, 23> b{};
        EXPECT_THROW_WITH_MESSAGE((array<bool, 3>{a}), "too large");
        EXPECT_THROW_WITH_MESSAGE((array<Elem, 1>{b}), "too large");
    }
}

TEST(OTL_ARRAY, Print)
{
    {
        const array<int, 5> arr(42);
        testing::internal::CaptureStdout();
        print(arr);
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "[42,42,42,42,42]\n");
    }
    {
        const array<float, 2> arr(1);
        testing::internal::CaptureStdout();
        print(arr);
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "[1,1]\n");
    }
    {
        const array<bool, 2> arr(true);
        testing::internal::CaptureStdout();
        print(arr);
        ASSERT_EQ(testing::internal::GetCapturedStdout(), "[true,true]\n");
    }
}

TEST(OTL_ARRAY, Contains)
{
    {
        array<int, 10> arr(42);
        arr[3] = 13;
        ASSERT_TRUE(contains(arr, 13));
        ASSERT_FALSE(contains(arr, 0));
    }
    {
        const array<Elem, 5> arr(Elem{1});
        ASSERT_TRUE(contains(arr, Elem{1}));
        ASSERT_FALSE(contains(arr, Elem{111}));
    }
    {
        const array<bool, 3> arr(false);
        ASSERT_TRUE(contains(arr, false));
    }
    {
        const array<bool, 3012> arr(false);
        ASSERT_FALSE(contains(arr, true));
    }
    {
        const array<bool, 123> arr(false);
        ASSERT_FALSE(contains(arr, true));
    }
}

} // namespace atl

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}