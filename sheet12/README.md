# Sheet 11 - Inheritance

## AST - Abstract Syntax Tree

An _abstract syntax tree (AST)_ is the standard data structure that is used to represent the syntactic structure
of strings written in any formal language. In an AST each node represents a syntactic construct,
such as an if-then-else statement, an arithmetic expression, or a single literal.
The children of an AST node represent the operands of a syntactic construct,
for example the left- and right-hand side of a binary addition.

One of the ways to implement AST nodes is via a class hierarchy.
Most nodes can have many different types of nodes as children.
In this exercise you will implement an AST for simple arithmetic expressions on
`double`s. Your AST should be able to represent the following operations:

- unary plus `+expr` (class `UnaryPlus`);
- unary minus `-expr` (class `UnaryMinus`);
- addition `expr1 + expr2` (class `Add`);
- subtraction `expr1 - expr2` (class `Subtract`);
- multiplication `expr1 * expr2` (class `Multiply`);
- division `expr1 / expr2` (class `Divide`);
- power `expr1 ^ expr2` (class `Power`).

It makes sense to create base classes `Unary` and `Binary` to capture operations on single operands and two operands,
respectively.

These operations can either operate on `double` constants, or on named parameters.
That is, in addition to the inner nodes listed above your AST implementation should support the following leaf nodes:

- `double` constant (class `Constant`);
- parameter (class `Parameter`).

For example, the expression `-P0 + 2 * (4 - P1)`, where `P0` and `P1` are two distinct parameters, could be represented
as a tree:

![ast tree diagram](./img/ast-diag.svg "AST tree diagram")

In all tasks you can assume that division by zero is never constructed.
You can assert against that in the `Divide` constructor.

### Part 1 &ndash; AST structure

Define the class hierarchy in `ast.hpp` and implement it in `ast.cpp`. Use `ASTNode` as the root of the hierarchy.

All classes must support returning their type with `getType() const`.

Unary AST nodes must:

- be constructible from a single child given as `std::unique_ptr<ASTNode>`,
- support `const ASTNode &getInput() const`, returning the const reference to the child node,
- support `std::unique_ptr<ASTNode> releaseInput()`, moving the child node to the caller.

Binary AST nodes must:

- be constructible from a left and a right child given as `std::unique_ptr<ASTNode>`s,
- support `const ASTNode &getLeft() const` and analogous `getRight()`,
- support `std::unique_ptr<ASTNode> releaseLeft()` and analogous `releaseRight()`.

The leaf nodes need special treatment. `Constant` must:

- be constructible from a single `double` value (the value of the constant),
- support `double getValue() const` returning its value;

and `Parameter` must:

- be constructible from the parameter index `size_t`,
- support `size_t getIndex() const` returning the index.

### Part 2 &ndash; visitor pattern

We would like to perform two different operations on the AST. First, given a context containing values of all parameters
we'd like to actually evaluate an expression. Second, we'd like to print the expression as a string.

Those two operations need to visit every node in the tree and apply some logic. While we could just define
`evaluate` and `format` functions on `ASTNode`, this is not the best design. First, adding any new operation like that
would have to change every single class in our hierarchy to support the new logic. Second, users from the outside
of our library are locked to the `public` API and have no easy way to add their own operations visiting the tree.
Fortunately, there's the [Visitor pattern](https://refactoring.guru/design-patterns/visitor).

The idea is to declare an `ASTVisitor` class that can `visit` any concrete subtype of `ASTNode`,
so`visit(const Constant &)`, `visit(const Add &)`, etc.
Then, by deriving from that class and overriding relevant `visit` methods, we can implement any logic we want.
The `ASTNode` itself needs a `void accept(ASTVisitor &visitor) const` method
whose implementation should always be trivial:

```cpp
visitor.visit(*this);
```

and dispatch to the appropriate `visit` function.

You should implement those functions and then two concrete visitors:

#### `EvaluateASTVisitor`

Taking an `EvaluationContext` (defined and implemented for you already in `evaluation_context.{hpp,cpp}`),
which is guaranteed to contain the values of all parameters, it should visit every node and evaluate the
value of its subexpression. For example, evaluating `(((P1 - P0) * (P0 + P1)) / 2.0) ^ 2.0`
under `P0 = 2.0`, `P1 = 4.0` should yield `36.0`.

Use the scaffolding in `evaluate_ast_visitor.hpp` to define the `EvaluateASTVisitor` class and put its implementation
in `evaluate_ast_visitor.cpp`. To evaluate `Power` you can use `std::pow` defined in `<cmath>`.

#### `FormatASTVisitor`

In `ast_formatter.hpp` you will find a specialization of `std::formatter<ASTNode, char>` for printing `ASTNode`
with the standard `<print>` facilities. It is defined wrt. to the `FormatASTVisitor` class, which you should
define using the scaffolding in `format_ast_visitor.hpp` (you can put implementation details in
`format_ast_visitor.cpp`,
although, due to template usage, this one will most likely be header-only).

The `FormatASTVisitor` has a template parameter `FmtIter`, which is guaranteed to be an `std::output_iterator`
for `char` values. You should be familiar with printing into an `out` iterator from the `formatting` task
in Sheet 07.

The expected format of unary expressions is `□(expr)` where `□` is `+` or `-` and `expr` is the underlying expression.
The expected format of binary expressions is `(expr1 □ expr2)` where `□` is the binary operator
(`+`, `-`, `*`, `/`, or `^`), `expr1` is the formatted left child, and `expr2` is the formatted right child.
Constants should be formatted with the standard formatting of `double` (so the result of `std::format("{}", value)`)
and parameters as their index prefixed with `P`.

This follows the same formatting as we used for declaring expressions throughout this problem statement.

## Mixins

C++ supports multiple inheritance, which is useful for modelling things like interfaces and mixins.
The idea of mixins is the ability to "mix in" various common behavioral patterns into classes
simply by deriving from a mixin. To avoid the diamond problem, mixins have no instance state, only
behavior (methods, member types, etc.). You are going to implement a few mixins to facilitate the usage of classes
defined in `usage.hpp`.

### `UniqueId<T>`

A simple example of a mixin is `UniqueId<T>`.
It declares that objects of the type deriving from it can be uniquely
identified by a value of type `T`. It should define:

- a pure virtual `const T &getId() const noexcept` function that returns the ID;
- the member typename `id_type` equal to `T`
- and equality comparison that compares the values of `getId` for rhs and lhs.

We can do more, we can provide hashing support based on the value of `getId`.
For that, [specialize the `std::hash` struct](https://en.cppreference.com/w/cpp/utility/hash)
for any type that derives `UniqueId<T>` where `T` is already hashable.

With that, any object deriving from `UniqueId` just needs to define `getId` and gets equality comparison
and hashing for free. And since the class carries no state, it can be freely added to any class without
triggering the diamond problem.

### CRTP or F-bounded universal qualification

The Curiously Recurring Template Pattern, as it is known to C++ developers (or F-bounded universal qualification
as known in lambda calculus) is a very complicated name for a syntactically simple language feature &ndash;
allowing base generic types to reference the derived type. Namely:

```cpp
template <class T>
class Base {};
class Derived : public Base<Derived> {};
```

This is a simple yet extremely powerful technique, as it allows us to use the concrete type
in signatures of functions in the base class, access its member types, etc.

As an example of CRTP, you will implement three mixins utilizing it.

#### `TypeId<T>`

When `T: TypeId<T>` it should receive a unique static type id, namely the call

```cpp
T::typeId()
```

should return a `size_t` value that is unique to the concrete type `T`. This value must not change
during runtime and be unique to `T`, in other words:

```
std::is_same_v<T, U> if and only if T::typeId() == U::typeId()
```

#### `CountInstances<T>`

When `T: CountInstances<T>`, all creations of an object of type `T` must be tracked in a static counter,
accessible via `T::getInstanceCount()` returning `size_t`.

```
T t1{};
ASSERT_EQ(1, T::getInstanceCount());
{
  T t2{t1};
  ASSERT_EQ(2, T::getInstanceCount());
}
// t2 destroyed
ASSERT_EQ(1, T::getInstanceCount());
```

The counters for each type are independent.

#### `BitRepr<T>`

A type can derive from `BitRepr<T>` to declare it is a simple type whose binary representation in memory
is its identity. In other words, it can be copied simply by copying all its bytes. This excludes any
types that, for example, own memory or have an indirection layer via a `shared_ptr`. However, for POCO types
whose fields are all simple, `BitRepr<T>` can provide a number of useful utility functions.

We assume that a type deriving from `BitRepr<T>` is default-constructible. You can `static_assert` that.

1. The copy and move constructors (and assignment operators)
   of `BitRepr<T>` should just copy all the bytes from the source.
2. It should provide `std::span<std::byte> asBytes() noexcept` (and its `const` equivalent,
   `std::span<const std::byte> asBytes() const noexcept`) that return the memory of the object as a span
   of bytes. Recall that [
   `std::span` is simply a pointer and a length](https://en.cppreference.com/w/cpp/container/span).
3. A `template <typename O> static void writeBinary(O out) const` function that takes an `std::output_iterator`
   and writes all the bytes of the object into it. This should be equivalent to manually calling `asBytes()` and
   copying the result into the `out`.
4. A complementary `template <typename R> static T readBinary(R range)` function that takes
   an `std::range` of bytes and constructs a new instance of `T` from the bytes in the range.
   You can assume the range of bytes is a valid representation of `T` (obtained e.g. from `writeBinary`);
   the behavior is undefined otherwise.
5. `std::string writeBase64() const` that produces a [Base64](https://en.wikipedia.org/wiki/Base64)
   representation of the objects bytes. The Base64 encoding and decoding algorithm is provided for you
   in `base64.hpp` and should not be modified.
6. A complementary `template <typename R> static T readBase64(R range)` function that takes an `std::range`
   of the Base64 representation and constructs a new instance of `T` from it. Again, the decoding algorithm
   is provided for you and you should assume the Base64 representation is valid for `T`.

The mixins should be put into the `mixins` namespace in `mixins.hpp`, with implementation in `mixins.cpp`.

### Usage

To help make the mixins more concrete, in `usage.hpp` you will find an example of three different
classes that inherit from the mixins you must define. To make the example more like the real world,
it contains absolutely no documentation.

All tests in `test_usage.cpp` are performed wrt. these three classes.

You may not modify any files aside from `mixins.hpp` and `mixins.cpp`.