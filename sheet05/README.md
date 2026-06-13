# Sheet 05 - Pointers, references, arrays

This weeks homework consists of two tasks, you are only rewarded the points for the exercise if you complete __both__!

## Arithmetic

You're tasked with inventing numbers! There are two directions you can take this in &ndash; using your reason or your
*_imagination_*.

Reason leads you to rational numbers. These are numbers of the form $\frac{p}{q}$ where $p$ and $q$ are integers.
Such numbers have a _canonical form_ where $p$ and $q$ are coprime; for example, $\frac{2}{4}$ in the canonical form
is $\frac{1}{2}$.

The power of imagination leads you to imaginary numbers, also known as complex numbers. These are numbers of the form
$x + yi$ where $i$ is the Power of Your Imagination. They have a number of fun properties, among other things they can
be interpreted as a vector $(x, y)$ in the two-dimensional *complex plane*.

Your task is to create two classes, `Rational` and `Complex`, representing the two kinds of numbers. The classes should
be defined in `rational.hpp` and `complex.hpp` with implementations in `rational.cpp` and `complex.cpp`, respectively.
The header files contain a scaffolding with doc comments for all the members that are expected to exist, and these are
further listed below.

### `Rational`

- Three constructors:
    - parameterless, creating the value of zero;
    - single-parameter `int64_t` representing conversion from integers to rationals;
    - two `int64_t` parameters for the numerator and denominator.
- Methods `num` and `den` returning the numerator and denominator, respectively, as `int64_t`. *The result must be
  canonical.*
- An explicit conversion to `double`.
- Method `inv` returning the inverse of the rational.
- Unary `-` and `+` operators.
- Arithmetic operators `-`, `+`, `*`, `/` on pairs of rationals.
- Three-way comparison `<=>` and equality `==`.

### `Complex`

- Three constructors:
    - parameterless, creating the value of zero;
    - single-parameter `double` representing conversion from reals to complex numbers;
    - two `double` parameters for the real and imaginary component.
- Methods `real` and `image` returning the real and imaginary component, respectively, as `double`.
- Method `abs` returning the absolute value of a complex number, also known as its modulus ($\sqrt{x^2 + y^2}$).
- Method `norm` returning the field norma of a complex number($x^2 + y^2$).
- Method `arg` returning the argument of a complex number, also known as its phase, in radians
  (the angle $\phi$ when in the polar form $r(\cos{\phi} + i \sin{\phi})$).
- Method `conj` returning the complex conjugate.
- Unary `-` and `+` operators.
- Arithmetic operators `-`, `+`, `*`, `/` on pairs of complex numbers.
- Equality comparison `==`.

## Disjoint Sets

A _disjoint-set data structure_, also known as a union-find data structure,
is a useful tool in many algorithmic problems. It is represented as a forest, and each disjoint set is a single
tree. It allows efficiently joining (unioning) two sets and checking if two elements belong to the same set.
You can read more about the implementations [here](https://en.wikipedia.org/wiki/Disjoint-set_data_structure) or in your
favorite edition of Introduction to Algorithms.

You are given a scaffolding of a `DisjointSets` class with its public API. Your task is to design its `private`
internals and then implement the API in `disjoint_sets.cpp`. The implementation needs to be reasonably efficient
&ndash; searching the entire forest for each `find` or `union` call is sure to time out!