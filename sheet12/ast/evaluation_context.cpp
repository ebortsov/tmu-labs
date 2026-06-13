#include "evaluation_context.hpp"
#include <cassert>

namespace ast {
double EvaluationContext::getParameter(size_t index) const {
  assert(index < parameters.size());
  return parameters[index];
}
void EvaluationContext::pushParameter(double value) { parameters.push_back(value); }
} // namespace ast
