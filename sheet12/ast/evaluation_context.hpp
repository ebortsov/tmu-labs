#pragma once
#include <vector>

namespace ast {
/// Container that contains all parameters used in the evaluation
class EvaluationContext {
public:
  /// Get the value of the parameter with the given index
  double getParameter(std::size_t index) const;
  /// Push a value for the next parameter
  void pushParameter(double value);

private:
  /// The parameters
  std::vector<double> parameters;
};
} // namespace ast
