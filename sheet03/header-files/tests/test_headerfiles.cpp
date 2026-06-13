#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

extern void entry();
extern int magic;
TEST(TestHeaderFiles, RunMagic3) {
    magic = 3;
    constexpr const char* expectedOutput = "Addition: 15\nSubtraction: 5\nMultiplication: 50\nDivision: 2\n42: 42\n3All operations are successful!\n";
    testing::internal::CaptureStdout();
    entry();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expectedOutput);
}

TEST(TestHeaderFiles, RunMagic7) {
    magic = 7;
    constexpr const char* expectedOutput = "Addition: 15\nSubtraction: 5\nMultiplication: 50\nDivision: 2\n42: 42\n7All operations are successful!\n";
    testing::internal::CaptureStdout();
    entry();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), expectedOutput);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
