#include "../ast.hpp"
#include "../ast_formatter.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <utility>

using namespace ast;

TEST(TestFormatASTVisitor, Constant) {
  std::unique_ptr<ASTNode> node = std::make_unique<Constant>(1);
  std::string s = std::format("{}", *node);
  EXPECT_EQ("1", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Parameter) {
  std::unique_ptr<ASTNode> node = std::make_unique<Parameter>(42);
  std::string s = std::format("{}", *node);
  EXPECT_EQ("P42", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, UnaryPlus) {
  std::unique_ptr<ASTNode> node = std::make_unique<UnaryPlus>(std::make_unique<Parameter>(42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(+P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, UnaryMinus) {
  std::unique_ptr<ASTNode> node = std::make_unique<UnaryMinus>(std::make_unique<Parameter>(42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(-P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Add) {
  auto p1 = std::make_unique<Parameter>(1);
  auto p42 = std::make_unique<Parameter>(42);
  std::unique_ptr<ASTNode> node = std::make_unique<Add>(std::move(p1), std::move(p42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(P1 + P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Subtract) {
  auto p1 = std::make_unique<Parameter>(1);
  auto p42 = std::make_unique<Parameter>(42);
  std::unique_ptr<ASTNode> node = std::make_unique<Subtract>(std::move(p1), std::move(p42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(P1 - P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Multiply) {
  auto p1 = std::make_unique<Parameter>(1);
  auto p42 = std::make_unique<Parameter>(42);
  std::unique_ptr<ASTNode> node = std::make_unique<Multiply>(std::move(p1), std::move(p42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(P1 * P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Divide) {
  auto p1 = std::make_unique<Parameter>(1);
  auto p42 = std::make_unique<Parameter>(42);
  std::unique_ptr<ASTNode> node = std::make_unique<Divide>(std::move(p1), std::move(p42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(P1 / P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Power) {
  auto p1 = std::make_unique<Parameter>(1);
  auto p42 = std::make_unique<Parameter>(42);
  std::unique_ptr<ASTNode> node = std::make_unique<Power>(std::move(p1), std::move(p42));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(P1 ^ P42)", s);
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Nested) {
  auto p0 = std::make_unique<Parameter>(0);
  auto p1 = std::make_unique<Parameter>(1);
  auto p2 = std::make_unique<Parameter>(2);
  auto p3 = std::make_unique<Parameter>(3);
  std::unique_ptr<ASTNode> node1 = std::make_unique<Add>(std::move(p0), std::move(p1));
  std::unique_ptr<ASTNode> node2 = std::make_unique<Multiply>(std::move(p2), std::move(p3));
  std::unique_ptr<ASTNode> node = std::make_unique<Power>(std::move(node1), std::move(node2));
  node = std::make_unique<UnaryMinus>(std::move(node));
  std::string s = std::format("{}", *node);
  EXPECT_EQ("(-((P0 + P1) ^ (P2 * P3)))", s);
}
