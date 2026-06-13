#pragma once
#include <memory>

namespace ast {
class ASTVisitor;

/// Base class for AST nodes
class ASTNode {
public:
  /// All possible types of ASTNodes
  enum class Type : uint8_t { UNARY_PLUS, UNARY_MINUS, ADD, SUBTRACT, MULTIPLY, DIVIDE, POWER, CONSTANT, PARAMETER };

  /// Get the type of this node
  Type getType() const;

  /// Accept an ASTVisitor
  virtual void accept(ASTVisitor &visitor) const;

  /// Destructor
  virtual ~ASTNode() = default;
};

/// Base class for unary nodes
class UnaryASTNode {};

/// Base class for binary nodes
class BinaryASTNode {};

/// ASTNode for unary plus
class UnaryPlus {};

/// ASTNode for unary minus
class UnaryMinus {};

/// ASTNode for addition
class Add {};

/// ASTNode for subtraction
class Subtract {};

/// ASTNode for multiplication
class Multiply {};

/// ASTNode for division
class Divide {};

/// ASTNode for raising to a power
class Power {};

/// ASTNode for a constant
class Constant {};

/// ASTNode for a parameter
class Parameter {};
} // namespace ast