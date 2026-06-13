#pragma once

namespace arithmetic {
/// A complex number
///
/// A number of the form z = x + iy
class Complex {
    /// Default constructor - the zero value.
    ???;
    /// Construct a complex number with only a real component.
    /// @param real The real component.
    ???;
    ///  Construct a complex number from a real and an imaginary component.
    /// @param real The real component.
    /// @param imag The imaginary component.
    ???;

    /// Real component of the number.
    /// @return The real component as a double.
    ???;
    /// Imaginary component of the number.
    /// @return The coefficient of the imaginary component of the number.
    ???;

    /// The absolute value, or modulus, of the complex number.
    ///
    /// This is equivalent to the distance of the number from zero on the complex plane.
    /// @return The absolute value of the number.
    ???;
    /// The field norm of the number.
    ///
    /// The field norm of a complex number is defined as the sum of the squares of its coefficients.
    /// @return The value of the field norm of the number.
    ???;
    /// The argument, or phase, of the complex number, in radians.
    ///
    /// This is the angle of the number with the positive real axis on the complex plane.
    /// @return The argument (in radians) of the number.
    ???;

    /// The complex conjugate.
    ///
    /// This is the reflection of the number about the real axis on the complex plane.
    /// @return The complex conjugate of the number.
    ???;

    /// Negate the number.
    ///
    /// This is the reflection of the number about the zero point of the complex plane.
    /// @return The negation of the number.
    ???;
    /// Unary plus
    ///
    /// This is a no-op.
    /// @return The same number.
    ???;

    /// Subtract another complex number.
    /// @param other The number to subtract.
    /// @return The result of subtracting this number from the other.
    ???;
    /// Add another complex number.
    /// @param other The number to add.
    /// @return The result of adding this number from the other.
    ???;
    /// Multiply by another complex number.
    /// @param other The number by which to multiply.
    /// @return The result of multiplying this number by the other.
    ???;
    /// Divide by another complex number.
    ///
    /// Dividing by zero is undefined behavior.
    /// @param other The number to divide by.
    /// @return The result of dividing this number by the other.
    ???;

    /// Compare another complex number for equality.
    /// @param other Number to compare to.
    /// @return True if this is equal to other; otherwise, false.
    ???;
};
} // namespace arithmetic