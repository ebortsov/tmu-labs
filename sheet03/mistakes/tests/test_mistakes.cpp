#include <print>
#include <sstream>
#include <vector>
#include <gtest/gtest.h>

extern void inspect_vector(std::vector<int> v);
void log(int line, std::string file, std::string msg) {
    if (file.size() < 25) return;
    std::println("...{}:{} - {}", file.substr(file.size() - 21), line, msg);
}
//---------------------------------------------------------------------------
TEST(TestMistakes, Compute1) {
    constexpr const char* expectedOutput = "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "Result = 5 21\n";
    testing::internal::CaptureStdout();
    inspect_vector({1, 2, 3, 4, 5, 6});

    ASSERT_EQ(testing::internal::GetCapturedStdout(), expectedOutput);
} //---------------------------------------------------------------------------
TEST(TestMistakes, Compute2) {
    constexpr const char* expectedOutput = "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "Result = 18 38\n";
    testing::internal::CaptureStdout();
    inspect_vector({19, 1, 4, 6, 8});
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expectedOutput);
}
//---------------------------------------------------------------------------
TEST(TestMistakes, Compute3) {
    constexpr const char* expectedOutput = "...mistakes/mistakes.cpp:28 - iterating the vector\n"
                                           "Result = 0 1\n";
    testing::internal::CaptureStdout();
    inspect_vector({1});
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expectedOutput);
}
//---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
