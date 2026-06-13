#include "../ast.hpp"
#include "../evaluation_context.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <utility>

using namespace ast;
using namespace std;

TEST(TestAST, Constant) {
  auto node = make_unique<Constant>(3);
  ASSERT_EQ(ASTNode::Type::CONSTANT, node->getType());
  ASSERT_EQ(3, node->getValue());
}
//---------------------------------------------------------------------------
TEST(TestAST, Parameter) {
  auto node = make_unique<Parameter>(0);
  ASSERT_EQ(ASTNode::Type::PARAMETER, node->getType());
  ASSERT_EQ(0, node->getIndex());
}
//---------------------------------------------------------------------------
TEST(TestAST, UnaryMinus) {
  auto leaf = make_unique<Constant>(3);
  auto expected = leaf.get();
  auto node = make_unique<UnaryMinus>(std::move(leaf));
  ASSERT_EQ(ASTNode::Type::UNARY_MINUS, node->getType());
  ASSERT_EQ(&node->getInput(), expected);
}
//---------------------------------------------------------------------------
TEST(TestAST, UnaryPlus) {
  auto leaf = make_unique<Constant>(3);
  auto expected = leaf.get();
  auto node = make_unique<UnaryPlus>(std::move(leaf));
  ASSERT_EQ(ASTNode::Type::UNARY_PLUS, node->getType());
  ASSERT_EQ(&node->getInput(), expected);
}
//---------------------------------------------------------------------------
TEST(TestAST, Add) {
  auto left = make_unique<Constant>(3);
  auto right = make_unique<Constant>(7);
  auto expectedLeft = left.get();
  auto expectedRight = right.get();
  auto node = make_unique<Add>(std::move(left), std::move(right));
  ASSERT_EQ(ASTNode::Type::ADD, node->getType());
  ASSERT_EQ(&node->getLeft(), expectedLeft);
  ASSERT_EQ(&node->getRight(), expectedRight);
}
//---------------------------------------------------------------------------
TEST(TestAST, Subtract) {
  auto left = make_unique<Constant>(3);
  auto right = make_unique<Constant>(7);
  auto expectedLeft = left.get();
  auto expectedRight = right.get();
  auto node = make_unique<Subtract>(std::move(left), std::move(right));
  ASSERT_EQ(ASTNode::Type::SUBTRACT, node->getType());
  ASSERT_EQ(&node->getLeft(), expectedLeft);
  ASSERT_EQ(&node->getRight(), expectedRight);
}
//---------------------------------------------------------------------------
TEST(TestAST, Multiply) {
  auto left = make_unique<Constant>(3);
  auto right = make_unique<Constant>(7);
  auto expectedLeft = left.get();
  auto expectedRight = right.get();
  auto node = make_unique<Multiply>(std::move(left), std::move(right));
  ASSERT_EQ(ASTNode::Type::MULTIPLY, node->getType());
  ASSERT_EQ(&node->getLeft(), expectedLeft);
  ASSERT_EQ(&node->getRight(), expectedRight);
}
//---------------------------------------------------------------------------
TEST(TestAST, Divide) {
  auto left = make_unique<Constant>(3);
  auto right = make_unique<Constant>(7);
  auto expectedLeft = left.get();
  auto expectedRight = right.get();
  auto node = make_unique<Divide>(std::move(left), std::move(right));
  ASSERT_EQ(ASTNode::Type::DIVIDE, node->getType());
  ASSERT_EQ(&node->getLeft(), expectedLeft);
  ASSERT_EQ(&node->getRight(), expectedRight);
}
//---------------------------------------------------------------------------
TEST(TestAST, Power) {
  auto left = make_unique<Constant>(3);
  auto right = make_unique<Constant>(7);
  auto expectedLeft = left.get();
  auto expectedRight = right.get();
  auto node = make_unique<Power>(std::move(left), std::move(right));
  ASSERT_EQ(ASTNode::Type::POWER, node->getType());
  ASSERT_EQ(&node->getLeft(), expectedLeft);
  ASSERT_EQ(&node->getRight(), expectedRight);
}