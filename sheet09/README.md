# Sheet 08 - Iterators and embracing the STL (II)

### Dispatch

Sometimes it is necessary to dispatch a function call based on some runtime information. We have seen some approaches to that already (based on a cascading-if or a switch-table) and also will see further ones later (inheritance).
Functional programming can be used to mimic a form of dynamic dispatching of functions according to the dynamic type of an object.
An object can store function pointers to different implementations of a function and, thus, expose different behaviour based on the runtime type.

For this weeks homework, we will implement a common example, that is often used with inheritance - but with function pointers instead: Evaluating expression-trees.

The expression class should have two publicly visible methods, which print the tree and evaluate the represented expression. 
Internally, they should store two function pointers to the exact implementation of the respective methods, along side with the members required to accomodate for the tree structure of the expression.
In total the size of an `Expr` should be at most the size of 4 pointers.
> Note: use a similar setup as for the _binary-tree_ homework two weeks ago, with up to two (potentially empty) children for each expression node.

An expression tree consists of computation nodes and parameter leafs, which reference a numbered parameter.

It should expose static methods for constructing Addition(Add), Substraction(Sub), Multiplication(Mul), Division(Div) and Unary Minus(Minus) expressions, which take their respective number of arguments as inputs. 
Furthermore, it should provide a static templated method to construct parameter references. 
They should be composable in the following manner:
```cpp
Expr::Add(Expr::Add(Expr::Param<0>(), Expr::Param<0>()), Expr::Param<0>())
```

The `print` method should print the matchematical expression in in-order notation, parameters are printed as `[idx]` and we wrap brackets around all expressions to make the evaluation order clear:
```cpp
Expr::Minus(Expr::Param<0>())->print() // "-[0]"
Expr::Add(Expr::Param<0>(), Expr::Param<0>())->print() // "([0]+[0])"
Expr::Add(Expr::Param<1>(), Expr::Add(Expr::Param<0>(), Expr::Param<0>()))->print() // "([1]+([0]+[0]))"
```

The `eval` method takes a reference to an `EvalCtx` to retrieve values for the parameters during evaluation.
The `EvalCtx` provides two methods for storing and retrieving parameter values that should be implemented as well.
Furthermore, the retrieval method should raise a `std::runtime_error` with the message "invalid parameter".

The evaluation of an expression-tree evaluates the underlying expression. You don't have to care about overflows etc. and just assume the default behaviour of operations on the correct-width integers. 
Only exceptions are divisions, where the evaluation should raise a `std::runtime_error` with the message "division by zero" if a divion by zero would occur.


### Dataprocessing

To become familiar with the STL algorithm library, we will process some data according to a specification.
The `process` methods takes two input DataChunks with column-wise storage and processes them the following:

```
       (in0)         (in1)
        LHS           RHS
       /   \         /   \
(1) part0  part1  part0  part1
     |       |      |      |
(2) sort   sort   sort   sort
     |       |      |      |
     \        \     /     /
(3)   ----,----\---      /
          |     ----,---/
           \       /
(4)         ---,--
               |
(5)          dedup
               |
(6)          group
               |
(7)          sort
               |
(8)           top
```

(1) hash-partition the rows of both chunks according to their hash-values (even/uneven hash-value) of the whole row.
    + The hash of a row is obtained by using `std::hash` on the individual cell-values of the columns and combining those partial hashes with the provided `hash_combine` method.
    + Split up the rows into two partitions according to the last bit of their hash
(2) sort each partition of each chunk ascending according to the `sort_merge_column` (ordering within a group of equal values should be maintained)
(3) merge the partitions of both chunks in the following way (left-semi-join):
    - even-valued-hash partition of first chunk with even-valued-hash partition of second chunk (same for uneven-hash-value partitions)
    - each row from the left-hand-side matches with each row from the right-hand-side where the value of the `sort_merge_column` in the left is equal to the values in the `sort_merge_column` on the right
    - with multiplicity: e.g. each value "1" on the left matches with all "1"s on the right (two "1"s on the left and ten "1"s on the right result in twenty resulting rows)
    - only the columns of the left-hand-side are pushed to the output
    - unmatched rows are discarded
(4) combine the output of both partition-wise merge operations by appending the results from the uneven-hash-value-partition to the even ones.
(5) deduplicate the rows (according to `unique_column`) - the first row, that contains a value, survives.
(6) group the values by the `grouping_column` and sum up the occurences of each value
(7) order the resulting groups according to the number of matches (descending) and use the row number as tie-breaker (ascending)
(8) println the `topN` result-combinations of `<key>: <nr-matches>@<address>` (print the key, nr-matches and the address of the first occurence of the value in the data)
> Note: for printing the pointer of the address use the default formatting and cast the pointer to an `uintptr_t`

Throughout the whole processing, try to copy as little data around as possible - use iterators where possible/required.
The tests should provide you with a variety of sample input-outputs.

When not specified differently, each individual step should presever the order of the elements. 
You don't have to worry about invalid column indices. Usage of the STL is explicitly allowed - some of the steps have a 1:1 corresponding algorithm definition is the STL - others don't.
You can not modify the `processing.hpp`!