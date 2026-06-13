#include <cstdint>
#include <memory>
#include <print>
#include <stdexcept>
#include <unordered_map>

class EvalCtx
{
public:
    /// Retrieve the parameter for a given index
    /// @param idx the index
    /// @return the parameter
    int64_t getParameter(size_t idx) const;
    /// Set the parameter for a given index
    /// @param idx the index
    /// @param param the parameter
    void setParameter(size_t idx, int64_t param);

private:
    // ...
};

class Expr
{
public:
    /// Print the represented math expression, with brackets around each binary expression and use [idx] for parameters:
    /// ```
    /// Expr::Minus(Expr::Param<0>())->print() // "-[0]"
    /// Expr::Add(Expr::Param<0>(), Expr::Param<0>())->print() // "([0]+[0])"
    /// Expr::Add(Expr::Param<1>(), Expr::Add(Expr::Param<0>(), Expr::Param<0>()))->print() // "([1]+([0]+[0]))"
    /// ```
    void print() const;
    /// Evaluate the represented math expressions. Parameters are retrieved from the context
    /// @param ctx the evaluation context
    /// @return the resulting value
    int64_t eval(const EvalCtx& ctx) const;

private:
    // ...
};
