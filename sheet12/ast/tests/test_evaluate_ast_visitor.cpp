#include "../ast.hpp"
#include "../evaluate_ast_visitor.hpp"
#include "../evaluation_context.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <utility>

using namespace ast;

TEST(TestEvaluateASTVisitor, EvaluateConstant) {
  EvaluationContext context;
  Constant c(1.0);
  EXPECT_EQ(EvaluateAstVisitor::evaluate(c, context), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateUnaryPlus) {
  EvaluationContext context;
  auto node = std::make_unique<UnaryPlus>(std::make_unique<Constant>(1.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateUnaryMinus) {
  EvaluationContext context;
  auto node = std::make_unique<UnaryMinus>(std::make_unique<Constant>(1.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), -1.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateAdd) {
  EvaluationContext context;
  auto node = std::make_unique<Add>(std::make_unique<Constant>(1.0), std::make_unique<Constant>(2.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 3.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateSubtract) {
  EvaluationContext context;
  auto node = std::make_unique<Subtract>(std::make_unique<Constant>(1.0), std::make_unique<Constant>(2.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), -1.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateMultiply) {
  EvaluationContext context;
  auto node = std::make_unique<Multiply>(std::make_unique<Constant>(2.0), std::make_unique<Constant>(3.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 6.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateDivide) {
  EvaluationContext context;
  auto node = std::make_unique<Divide>(std::make_unique<Constant>(1.0), std::make_unique<Constant>(2.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 0.5);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluatePower) {
  EvaluationContext context;
  auto node = std::make_unique<Power>(std::make_unique<Constant>(2.0), std::make_unique<Constant>(3.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 8.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateParameter) {
  EvaluationContext context;
  context.pushParameter(1.0);
  auto node = std::make_unique<Parameter>(0);
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestEvaluateASTVisitor, EvaluateNested) {
  // evaluate: (((P1 - P0) * (P0 + P1)) / 2.0) ^ 2.0
  // where: P0 = 2.0, P1 = 4.0
  EvaluationContext context;
  context.pushParameter(2.0);
  context.pushParameter(4.0);
  std::unique_ptr<ASTNode> node1 =
      std::make_unique<Add>(std::make_unique<Parameter>(0), std::make_unique<Parameter>(1));
  std::unique_ptr<ASTNode> node2 =
      std::make_unique<Subtract>(std::make_unique<Parameter>(1), std::make_unique<Parameter>(0));
  std::unique_ptr<ASTNode> node = std::make_unique<Multiply>(std::move(node1), std::move(node2));
  node = std::make_unique<Divide>(std::move(node), std::make_unique<Constant>(2.0));
  node = std::make_unique<Power>(std::move(node), std::make_unique<Constant>(2.0));
  EXPECT_EQ(EvaluateAstVisitor::evaluate(*node, context), 36.0);
}