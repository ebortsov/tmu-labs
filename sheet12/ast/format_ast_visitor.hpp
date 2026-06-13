#pragma once
#include "ast.hpp"
#include "ast_visitor.hpp"

namespace ast {
/// Template formatting visitor for an ASTNode to specialize std::formatter.
template <class FmtIter>
class FormatAstVisitor : public ASTVisitor {
  /// Format a given node into a formatter output iterator.
  static FmtIter format(const ASTNode &node, FmtIter out);

  // Your implementation here.
};
} // namespace ast