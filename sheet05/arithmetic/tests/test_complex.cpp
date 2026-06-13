#include "../complex.hpp"
#include <cmath>
#include <gtest/gtest.h>

TEST(TestComplex, realImag) {
    const arithmetic::Complex a;
    const arithmetic::Complex b(42.0);
    const arithmetic::Complex c(1.0, 2.0);

    ASSERT_FLOAT_EQ(a.real(), 0);
    ASSERT_FLOAT_EQ(a.imag(), 0);

    ASSERT_FLOAT_EQ(b.real(), 42.0);
    ASSERT_FLOAT_EQ(b.imag(), 0);

    ASSERT_FLOAT_EQ(c.real(), 1.0);
    ASSERT_FLOAT_EQ(c.imag(), 2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, abs) {
    const arithmetic::Complex a(3.0, 4.0);

    ASSERT_FLOAT_EQ(a.abs(), 5.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, norm) {
    const arithmetic::Complex a(3.0, 4.0);

    ASSERT_FLOAT_EQ(a.norm(), 25.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, arg) {
    const arithmetic::Complex a(1.0, 0.0);
    const arithmetic::Complex b(0.0, 1.0);
    const arithmetic::Complex c(-1.0, 0.0);
    const arithmetic::Complex d(0.0, -1.0);

    ASSERT_FLOAT_EQ(a.arg(), 0.0);
    ASSERT_FLOAT_EQ(b.arg(), M_PI / 2);
    ASSERT_FLOAT_EQ(c.arg(), M_PI);
    ASSERT_FLOAT_EQ(d.arg(), -M_PI / 2);
}
//---------------------------------------------------------------------------
TEST(TestComplex, conj) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex c = a.conj();

    ASSERT_FLOAT_EQ(c.real(), 1.0);
    ASSERT_FLOAT_EQ(c.imag(), -2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, unaryMinus) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b = -a;

    ASSERT_FLOAT_EQ(b.real(), -1.0);
    ASSERT_FLOAT_EQ(b.imag(), -2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, unaryPlus) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b = +a;

    ASSERT_FLOAT_EQ(b.real(), 1.0);
    ASSERT_FLOAT_EQ(b.imag(), 2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, binaryPlus) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b(3.0, 4.0);
    const arithmetic::Complex c = a + b;

    ASSERT_FLOAT_EQ(c.real(), 4.0);
    ASSERT_FLOAT_EQ(c.imag(), 6.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, binaryMinus) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b(3.0, 4.0);
    const arithmetic::Complex c = a - b;

    ASSERT_FLOAT_EQ(c.real(), -2.0);
    ASSERT_FLOAT_EQ(c.imag(), -2.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, multiplication) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b(3.0, 4.0);
    const arithmetic::Complex c = a * b;

    ASSERT_FLOAT_EQ(c.real(), -5.0);
    ASSERT_FLOAT_EQ(c.imag(), 10.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, division) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b(3.0, 4.0);
    const arithmetic::Complex c = a / b;

    ASSERT_FLOAT_EQ(c.real(), 11.0 / 25.0);
    ASSERT_FLOAT_EQ(c.imag(), 2.0 / 25.0);
}
//---------------------------------------------------------------------------
TEST(TestComplex, comparison) {
    const arithmetic::Complex a(1.0, 2.0);
    const arithmetic::Complex b(3.0, 4.0);

    ASSERT_TRUE(a == a);
    ASSERT_TRUE(b == b);
    ASSERT_FALSE(a == b);

    ASSERT_FALSE(a != a);
    ASSERT_FALSE(b != b);
    ASSERT_TRUE(a != b);
}
//---------------------------------------------------------------------------
