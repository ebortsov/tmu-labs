#pragma once

namespace arithmetic {
/// A rational number.
///
/// A number of the form p/q for integer p and q.
class Rational {    /// Default constructor - the zero value.
    Rational();
    /// Construct a rational number equal to the given integer.
    /// @param value The integer value.
    ???;
    /// Construct a rational number from a numerator and a denominator.
    ///
    /// The returned rational is always in the canonical form.
    /// @param numerator The value of the numerator.
    /// @param denominator The value of the denominator.
    ???;

    /// Numerator of the number.
    /// @return The numerator.
    ???;
    /// Denominator of the number.
    /// @return The denominator.
    ???;

    /// Explicit conversion to double.
    ???;

    /// Inverse of the rational.
    ///
    /// Equivalent to 1 / this.
    /// @return The inverse of this number.
    ???;

    /// Negate the number.
    /// @return The negation of the number.
    ???;
    /// Unary plus
    ///
    /// This is a no-op.
    /// @return The same number.
    ???;

    /// Subtract another rational number.
    /// @param other The number to subtract.
    /// @return The result of subtracting this number from the other.
    ???;
    /// Add another rational number.
    /// @param other The number to add.
    /// @return The result of adding this number to the other.
    ???;
    /// Multiply by another rational number.
    /// @param other The number by which to multiply.
    /// @return The result of multiplying this number by the other.
    ???;
    /// Divide by another rational number.
    ///
    /// Dividing by zero is undefined behavior.
    /// @param other The number to divide by.
    /// @return The result of dividing this number by the other.
    ???;

    /// Three-way comparison.
    /// @param other The number to compare to.
    /// @return The strong_ordering result of the comparison.
    ???;

    /// Equality comparison.
    ???;
};
} // namespace arithmetic
