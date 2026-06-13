#pragma once
#include "ast.hpp"
#include "evaluation_context.hpp"

namespace ast {
/// AST visitor that evaluates it under an EvaluationContext.
class EvaluateAstVisitor {
public:
  /// Evaluate an AST in a given context.
  static double evaluate(const ASTNode &ast, const EvaluationContext &context);
};
} // namespace ast