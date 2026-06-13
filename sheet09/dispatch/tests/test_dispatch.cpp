#include "../expression.hpp"
#include <gtest/gtest.h>

TEST(Dispatch, ClassSize)
{
    ASSERT_LE(sizeof(Expr), 4 * sizeof(void*));
}

TEST(Dispatch, BuildTree)
{
    auto expression = Expr::Add(Expr::Sub(Expr::Param<0>(), Expr::Param<1>()), Expr::Mul(Expr::Div(Expr::Add(Expr::Param<0>(), Expr::Param<0>()), Expr::Param<12>()), Expr::Param<(size_t)-1>()));
    ASSERT_TRUE(expression);
}

TEST(Dispatch, PrintTree1)
{
    auto expression = Expr::Add(Expr::Param<0>(), Expr::Minus(Expr::Param<1>()));
    testing::internal::CaptureStdout();
    expression->print();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "([0]+-[1])");
}

TEST(Dispatch, PrintTree2)
{
    auto expression = Expr::Add(Expr::Param<0>(), Expr::Mul(Expr::Param<1>(), Expr::Param<2>()));
    testing::internal::CaptureStdout();
    expression->print();
    ASSERT_EQ(testing::internal::GetCapturedStdout(), "([0]+([1]*[2]))");
}

TEST(Dispatch, EvalCtx)
{
    EvalCtx ctx;
    ctx.setParameter(0, 1);
    ctx.setParameter(1, 2);
    ASSERT_EQ(ctx.getParameter(0), 1);
    ASSERT_EQ(ctx.getParameter(1), 2);
    ctx.setParameter(-1, 42);
    ASSERT_EQ(ctx.getParameter(-1), 42);
}

TEST(Dispatch, EvalTree1)
{
    auto expression = Expr::Add(Expr::Param<0>(), Expr::Minus(Expr::Param<1>()));
    EvalCtx ctx;
    ctx.setParameter(0, 100);
    ctx.setParameter(1, 50);
    ASSERT_EQ(expression->eval(ctx), 50);
}

TEST(Dispatch, EvalTree2)
{
    auto expression = Expr::Add(Expr::Param<0>(), Expr::Mul(Expr::Param<1>(), Expr::Param<2>()));
    EvalCtx ctx;
    ctx.setParameter(0, 100);
    ctx.setParameter(1, 10);
    ctx.setParameter(2, 11);
    ASSERT_EQ(expression->eval(ctx), 210);
}

#define EXPECT_THROW_WITH_MESSAGE(statement, txt)     \
    try {                                             \
        statement;                                    \
        FAIL() << "Expected exception!";              \
    } catch (std::runtime_error & msg) {              \
        ASSERT_EQ(std::string_view(msg.what()), txt); \
    } catch (...) {                                   \
        FAIL() << "Expected runtime error!";          \
    }

TEST(Dispatch, DivisionByZero)
{
    auto expression = Expr::Div(Expr::Param<0>(), Expr::Mul(Expr::Param<1>(), Expr::Param<2>()));
    EvalCtx ctx;
    ctx.setParameter(0, 100);
    ctx.setParameter(1, 10);
    ctx.setParameter(2, 0);
    EXPECT_THROW_WITH_MESSAGE(expression->eval(ctx), "division by zero");
}

TEST(Dispatch, InvalidParameter)
{
    auto expression = Expr::Div(Expr::Param<0>(), Expr::Mul(Expr::Param<1>(), Expr::Param<2>()));
    EvalCtx ctx;
    ctx.setParameter(0, 100);
    ctx.setParameter(1, 10);
    ctx.setParameter(3, 0);
    EXPECT_THROW_WITH_MESSAGE(expression->eval(ctx), "invalid parameter");
}

TEST(Dispatch, EvalTree3)
{
    auto expression = Expr::Add(Expr::Sub(Expr::Param<0>(), Expr::Param<1>()), Expr::Mul(Expr::Div(Expr::Add(Expr::Param<0>(), Expr::Param<0>()), Expr::Param<12>()), Expr::Param<(size_t)-1>()));
    EvalCtx ctx;
    ctx.setParameter(0, 100);
    ctx.setParameter(1, 10);
    ctx.setParameter(12, 11);
    ctx.setParameter(-1, 167);
    ASSERT_EQ(expression->eval(ctx), 3096);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
