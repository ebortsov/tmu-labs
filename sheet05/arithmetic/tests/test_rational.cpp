#include "../rational.hpp"
#include <gtest/gtest.h>

TEST(TestRational, numDen) {
    const arithmetic::Rational a;
    const arithmetic::Rational b(42);
    const arithmetic::Rational c(42, 1);

    ASSERT_EQ(a.num(), 0);
    ASSERT_EQ(a.den(), 1);

    ASSERT_EQ(b.num(), 42);
    ASSERT_EQ(b.den(), 1);

    ASSERT_EQ(c.num(), 42);
    ASSERT_EQ(c.den(), 1);
}
//---------------------------------------------------------------------------
TEST(TestRational, canonicalization) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b(-1, -2);
    const arithmetic::Rational c(42, -1);

    ASSERT_EQ(a.num(), 12);
    ASSERT_EQ(a.den(), 5);

    ASSERT_EQ(b.num(), 1);
    ASSERT_EQ(b.den(), 2);

    ASSERT_EQ(c.num(), -42);
    ASSERT_EQ(c.den(), 1);
}
//---------------------------------------------------------------------------
TEST(TestRational, conversion) {
    const arithmetic::Rational a(84, 35);
    double v = static_cast<double>(a);

    ASSERT_FLOAT_EQ(v, 2.4);
}
//---------------------------------------------------------------------------
TEST(TestRational, inv) {
    const arithmetic::Rational a(-84, 35);
    const arithmetic::Rational b = a.inv();

    ASSERT_EQ(b.num(), -5);
    ASSERT_EQ(b.den(), 12);
}
//---------------------------------------------------------------------------
TEST(TestRational, unaryMinus) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b = -a;

    ASSERT_EQ(b.num(), -12);
    ASSERT_EQ(b.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, unaryPlus) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b = +a;

    ASSERT_EQ(b.num(), 12);
    ASSERT_EQ(b.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, binaryMinus) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b(6, 18);
    const arithmetic::Rational c = a - b;

    ASSERT_EQ(c.num(), 31);
    ASSERT_EQ(c.den(), 15);
}
//---------------------------------------------------------------------------
TEST(TestRational, binaryPlus) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b(6, 18);
    const arithmetic::Rational c = a + b;

    ASSERT_EQ(c.num(), 41);
    ASSERT_EQ(c.den(), 15);
}
//---------------------------------------------------------------------------
TEST(TestRational, multiplication) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b(6, 18);
    const arithmetic::Rational c = a * b;

    ASSERT_EQ(c.num(), 4);
    ASSERT_EQ(c.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, division) {
    const arithmetic::Rational a(84, 35);
    const arithmetic::Rational b(6, 18);
    const arithmetic::Rational c = a / b;

    ASSERT_EQ(c.num(), 36);
    ASSERT_EQ(c.den(), 5);
}
//---------------------------------------------------------------------------
TEST(TestRational, comparison) {
    const arithmetic::Rational a(1, 3);
    const arithmetic::Rational b(6, 18);
    const arithmetic::Rational c(1, 4);
    const arithmetic::Rational d(90071992547409902, 1);
    const arithmetic::Rational e(90071992547409901, 1);

    ASSERT_TRUE(a == a);
    ASSERT_TRUE(a <= a);
    ASSERT_FALSE(a < a);
    ASSERT_TRUE(a >= a);
    ASSERT_FALSE(a > a);
    ASSERT_EQ(a <=> a, std::strong_ordering::equal);

    ASSERT_TRUE(a == b);
    ASSERT_TRUE(a <= b);
    ASSERT_FALSE(a < b);
    ASSERT_TRUE(a >= b);
    ASSERT_FALSE(a > b);
    ASSERT_EQ(a <=> b, std::strong_ordering::equal);

    ASSERT_TRUE(b == a);
    ASSERT_TRUE(b <= a);
    ASSERT_FALSE(b < a);
    ASSERT_TRUE(b >= a);
    ASSERT_FALSE(b > a);
    ASSERT_EQ(b <=> a, std::strong_ordering::equal);

    ASSERT_FALSE(a == c);
    ASSERT_FALSE(a <= c);
    ASSERT_FALSE(a < c);
    ASSERT_TRUE(a >= c);
    ASSERT_TRUE(a > c);
    ASSERT_EQ(a <=> c, std::strong_ordering::greater);

    ASSERT_FALSE(c == a);
    ASSERT_TRUE(c <= a);
    ASSERT_TRUE(c < a);
    ASSERT_FALSE(c >= a);
    ASSERT_FALSE(c > a);
    ASSERT_EQ(c <=> a, std::strong_ordering::less);

    ASSERT_FALSE(d == e);
    ASSERT_FALSE(d <= e);
    ASSERT_FALSE(d < e);
    ASSERT_TRUE(d >= e);
    ASSERT_TRUE(d > e);
    ASSERT_EQ(d <=> e, std::strong_ordering::greater);
}
