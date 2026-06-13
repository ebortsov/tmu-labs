#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <gtest/gtest.h>

bool division_by_zero_flag = false;
void division_by_zero() {
    division_by_zero_flag = true;
}
std::vector<int32_t> compiled_program;
size_t program_idx = 0;
int32_t next() {
    if (program_idx < compiled_program.size())
        return compiled_program[program_idx++];
    return 0;
}

namespace simplevm {
extern int32_t execute();
}

//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static int32_t runProgram(std::string program) {
    std::stringstream programstream(program);
    compiled_program.clear();
    program_idx = 0;
    while (programstream)
        programstream >> compiled_program.emplace_back();
    return simplevm::execute();
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, EmptyRegister) {
    EXPECT_EQ(runProgram("0"), 0);
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, IRegs) {
    {
        SCOPED_TRACE("movi");
        ASSERT_EQ(runProgram("10 0 123\n0"), 123);
    }
    {
        SCOPED_TRACE("movi2");
        ASSERT_EQ(runProgram("10 1 123\n0"), 0);
    }
    {
        SCOPED_TRACE("loada");
        ASSERT_EQ(runProgram("10 0 123\n20 0\n0"), 123);
        ASSERT_EQ(runProgram("10 2 123\n20 2\n0"), 123);
    }
    {
        SCOPED_TRACE("storea");
        ASSERT_EQ(runProgram("10 0 123\n21 3\n0"), 123);
        ASSERT_EQ(runProgram("10 0 123\n21 0\n0"), 123);
        ASSERT_EQ(runProgram("10 0 123\n21 3\n10 0 456\n20 3\n0"), 123);
    }
    {
        SCOPED_TRACE("swapab");
        ASSERT_EQ(runProgram("10 0 123\n10 1 456\n22\n0"), 456);
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, IntArithmetic) {
    {
        SCOPED_TRACE("addi");
        EXPECT_EQ(runProgram("10 0 9\n10 1 10\n50\n0"), 19);
    }
    {
        SCOPED_TRACE("addi overflow");
        EXPECT_EQ(runProgram("10 0 2147483647\n10 1 1\n50\n0"), -2147483648);
        EXPECT_EQ(runProgram("10 0 2147483647\n10 1 2147483647\n50\n0"), -2);
    }
    {
        SCOPED_TRACE("addi underflow");
        EXPECT_EQ(runProgram("10 0 -2147483648\n10 1 -1\n50\n0"), 2147483647);
        EXPECT_EQ(runProgram("10 0 -2147483648\n10 1 -2147483648\n50\n0"), 0);
    }
    {
        SCOPED_TRACE("subi");
        EXPECT_EQ(runProgram("10 0 10\n10 1 9\n51\n0"), 1);
    }
    {
        SCOPED_TRACE("subi underflow");
        EXPECT_EQ(runProgram("10 0 -2147483648\n10 1 1\n51\n0"), 2147483647);
        EXPECT_EQ(runProgram("10 0 -2147483648\n10 1 -2147483648\n51\n0"), 0);
    }
    {
        SCOPED_TRACE("subi overflow");
        EXPECT_EQ(runProgram("10 0 2147483647\n10 1 -1\n51\n0"), -2147483648);
        EXPECT_EQ(runProgram("10 0 2147483647\n10 1 -2147483647\n51\n0"), -2);
    }
    {
        SCOPED_TRACE("muli");
        EXPECT_EQ(runProgram("10 0 2\n10 1 3\n53\n0"), 6);
    }
    {
        SCOPED_TRACE("muli overflow");
        EXPECT_EQ(runProgram("10 0 2147483647\n10 1 2\n53\n0"), -2);
        EXPECT_EQ(runProgram("10 0 2147483647\n10 1 2147483647\n53\n0"), 1);
    }
    {
        SCOPED_TRACE("muli underflow");
        EXPECT_EQ(runProgram("10 0 -2147483648\n10 1 2\n53\n0"), 0);
        EXPECT_EQ(runProgram("10 0 -2147483648\n10 1 2147483647\n53\n0"), -2147483648);
    }
    {
        SCOPED_TRACE("divi");
        EXPECT_EQ(runProgram("10 0 5\n10 1 2\n54\n0"), 2);
        EXPECT_EQ(runProgram("10 0 5\n10 1 2\n54\n22\n0"), 1);
        EXPECT_EQ(runProgram("10 0 -5\n10 1 -2\n54\n0"), 2);
        EXPECT_EQ(runProgram("10 0 -5\n10 1 -2\n54\n22\n0"), -1);
        EXPECT_EQ(runProgram("10 0 -5\n10 1 2\n54\n0"), -2);
        EXPECT_EQ(runProgram("10 0 -5\n10 1 2\n54\n22\n0"), -1);
        EXPECT_EQ(runProgram("10 0 5\n10 1 -2\n54\n0"), -2);
        EXPECT_EQ(runProgram("10 0 5\n10 1 -2\n54\n22\n0"), 1);
    }
}
//---------------------------------------------------------------------------
TEST(SimplevmTest, DivisionByZero) {
    {
        SCOPED_TRACE("divi");
        division_by_zero_flag = false;
        EXPECT_EQ(runProgram("10 0 123\n54\n"), 0);
        EXPECT_TRUE(division_by_zero_flag);
    }
}

//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
