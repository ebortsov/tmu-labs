#pragma once
#include "ast.hpp"
#include "ast_visitor.hpp"
#include "format_ast_visitor.hpp"
#include <algorithm>
#include <format>

namespace std {
/// Specialization to enable std::format with ASTNode
template <>
struct formatter<ast::ASTNode, char> {
  template <class ParseContext>
  constexpr typename ParseContext::iterator parse(ParseContext &ctx) {
    // Ignore all arguments
    auto it = ctx.begin();
    if (it == ctx.end()) return it;
    while (it != ctx.end() && *it != '}')
      ++it;
    return it;
  }

  template <class FmtContext>
  typename FmtContext::iterator format(const ast::ASTNode &node, FmtContext &ctx) const {
    typename FmtContext::iterator out = ctx.out();
    return ast::FormatAstVisitor<decltype(out)>::format(node, out);
  }
};
} // namespace std