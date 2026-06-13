#include "../formatting.hpp"
#include <format>
#include <gtest/gtest.h>

TEST(ObfuscatedText, format)
{
    EXPECT_EQ(std::format("{:2}", ObfuscatedText("abc def")), "a_c_e_");
    EXPECT_EQ(std::format("{:1}", ObfuscatedText("abc def")), "______");
    EXPECT_EQ(std::format("{:9}", ObfuscatedText("abc def")), "abcdef");
    EXPECT_EQ(std::format("{:100000}", ObfuscatedText("abc def")), "abcdef");
    EXPECT_EQ(std::format("{:3}", ObfuscatedText("abc def")), "ab_de_");
}

TEST(ProgressBar, formatFeatures)
{
    EXPECT_EQ(std::format("{:20}", ProgressBar(1.0)), "|##################|");
    EXPECT_EQ(std::format("{:}", ProgressBar(1.0)), "|########|");
    EXPECT_EQ(std::format("{:<}", ProgressBar(1.0)), "|#######>|");
    EXPECT_EQ(std::format("{:>}", ProgressBar(1.0)), "|#######>|");
    EXPECT_EQ(std::format("{:]>}", ProgressBar(1.0)), "[#######>]");
    EXPECT_EQ(std::format("{:[<}", ProgressBar(1.0)), "[#######>]");
    EXPECT_EQ(std::format("{:)<}", ProgressBar(1.0)), "(#######>)");
    EXPECT_EQ(std::format("{:(<}", ProgressBar(1.0)), "(#######>)");
    EXPECT_EQ(std::format("{:l2r(<}", ProgressBar(1.0)), "(#######>)");
    EXPECT_EQ(std::format("{:r2l(<}", ProgressBar(1.0)), "(<#######)");
    EXPECT_EQ(std::format("{:*r2l(<}", ProgressBar(1.0)), "(<*******)");
    EXPECT_EQ(std::format("{:=r2l(<}", ProgressBar(1.0)), "(<=======)");
}

TEST(ProgressBar, formatProgress)
{
    EXPECT_EQ(std::format("{:}", ProgressBar(0.5)), "|####    |");
    EXPECT_EQ(std::format("{:11}", ProgressBar(0.5)), "|####     |");
    EXPECT_EQ(std::format("{:12}", ProgressBar(0.5)), "|#####     |");

    EXPECT_EQ(std::format("{:}", ProgressBar(0.1)), "|        |");
    EXPECT_EQ(std::format("{:11}", ProgressBar(0.1)), "|         |");
    EXPECT_EQ(std::format("{:12}", ProgressBar(0.1)), "|#         |");
    EXPECT_EQ(std::format("{:13}", ProgressBar(0.1)), "|#          |");

    EXPECT_EQ(std::format("{:}", ProgressBar(0.9)), "|####### |");
    EXPECT_EQ(std::format("{:11}", ProgressBar(0.9)), "|######## |");
    EXPECT_EQ(std::format("{:12}", ProgressBar(0.9)), "|######### |");
    EXPECT_EQ(std::format("{:20}", ProgressBar(0.9)), "|################  |");
}

TEST(ProgressBar, formatminimal)
{
    EXPECT_EQ(std::format("{:3*r2l[>}", ProgressBar(1.0)), "[<]");
    EXPECT_EQ(std::format("{:3*r2l[<]}", ProgressBar(0.9)), "[ ]");
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
