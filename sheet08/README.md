# Sheet 07 - Iterators and embracing the STL

## Bitset

As we have seen already in last weeks exercise, booleans can be efficiently stored within the bits of a byte. Instead of statically size arrays, you will imlpement a dynamically growing _bitset_ this week.
Note that the C++ standard library already has `std::vector<bool>`,`std::bitset`, and similar types that you should __not__ use in your implementation (use the provideded `bytes` vector for storing the data).

Implement a class named `BitSet` in the `bitset` namespace, using the program scaffold contained in the subdirectory for this exercise. Your class should support at least the following operations:
- Constructors
    + Default constructor: Initialize an empty bitset.
    + Construction with a specified size: Initialize a bitset containing the specified number of `false` elements.
- Member functions
    + `size`: Return the total number of elements in the bitset.
    + `cardinality`: Return the number of `true` elements in the bitset.
    + `push_back`: Insert an element at the end of the bitset.
    + `front`: Access the first element.
    + `back`: Access the last element.
- Operators
    + Subscript: Access a specific element
Take care to use the most restrictive cv-qualifiers possible and define suitable `const` and non-`const` overloads for the `front` and `back` functions as well as for the subscript operator. 

The `const` overloads of `front`, `back`, and the subscript operator can simply return a `bool`. 
However, the non-`const` overloads must return an object that acts as a reference to an individual bit, so that it is possible to write, for example, `my_bitset[10] = false` or `my_bitset[10] = my_bitset[11]`. 
Since individual bits are not actually stored in a dedicated variable of type `bool` in our implementation, we cannot directly return a reference to an individual bit. 
Instead, you have to implement the nested class-type `BitReference` which should be used as the return value of the non-`const` overloads.
It should support the following operations:
+ Operators
    - Implicit conversion to `bool`
    - Assignment from `bool`
    - Assignment from `const BitReference&`

Since the helper class is __implicitly__ convertible to `bool`, it is valid to write code like `bool a = my_bitset.front()`. 
Since it also defines an assignment operator from `bool`, it is valid to write code like `my_bitset.front() = true`. 
Finally, since it defines an assignment operator from `const BitReference&`, it is valid to write code like `my_bitset.front() = my_bitset.back()`.

At this point, our bitset container is already useful but it is missing one important feature: We cannot use it in range-for statements. 
For this, we need to implement a suitable `iterator` along with some more functions.

Iterators are typically nested classes that implement the logic required to identify a specific element within a container along with operations to move the iterator. 

Implement a nested class `BitIterator` within the `BitSet` class. This class should have the following members:
- Public nested type aliases
    + `difference_type`: alias for `std::ptrdiff_t`
    + `value_type`: alias for `bool`
    + `reference`: alias for `BitReference`
    + `iterator_category`: alias for `std::bidirectional_iterator_tag`
- Operators
    + Equality: Check if two iterators point to the same bit
    + Dereference: Return a `BitReference` referencing the current bit
    + Pre- and Post-Increment (`++it` and `it++`): Move the iterator to the next bit
    + Pre- and Post-Decrement (`--it` and `it--`): Move the iterator to the previous bit

Add `begin` and `end` functions to the `BitSet` class which return a `BitIterator` that points to the first bit, and past the last bit, respectively.

The `BitIterator` class follows some specific requirements that are imposed on iterators by the C++ standard through the `std::bidirectional_iterator` concept.
Together with the `begin` and `end` functions, this allows us to use the `BitSet` class in range-for statements (take a look at the test cases for this exercise, for example).
> Note: the dereference function must be declared as const to satisfy the concept.

As it is possible to change individual elements in the bitset through the `BitIterator` class (e.g. `*bitset.begin() = false`), they cannot be used with `const` instances.
In order to support iteration over `const` `BitSet`s, we need to implement another suitable iterator class which does not permit modification of the elements.

Implement a nested class `ConstBitIterator` within the `BitSet` class, which publicly exposes the same members as the `BitIterater` class from before, with the following exceptions.
- The `reference` nested type alias should be an alias for `bool`
- The dereference operator should return `bool`

Add `const`-qualified `begin` and `end` functions to the `BitSet` class which return objects of type `ConstBitIterator` that point to the first bit, and past the last bit, respectively.
This allows us to also use `const` `BitSet`s in range-for statements.

## Formatting

Throughout the homework assignments we have seen many different custom types already.
For debugging or visualization you might have arrived at the situation, where you wanted to print them, but `std::print` refused to do so.
For a custom type to be formattable by the default print/format, there has to be a matching specialization of the `std::formatter` struct (https://en.cppreference.com/w/cpp/utility/format/formatter).

We will implement two in this homework:

### Obfuscated Text
Implement a `std::formatter` specialization for the obfuscated text class. It should print the container text but remove all spaces. 
Furthermore, it should replace every n-th character in the output with a '_'. The n is specified in the format specification.
As the format parser must be evaluatable at compile-time, use `std::from_chars` to convert the string to a number (other functions like `atoi` or `strtoi` do not satisfy the compile-time requirement!).
Example:
```
std::format("{:2}", ObfuscatedText("abc def")) // a_c_e_
std::format("{:1}", ObfuscatedText("abc def")) // ______
std::format("{:9}", ObfuscatedText("abc def")) // abcdef
std::format("{:100000}", ObfuscatedText("abc def")) // abcdef
```

### Progress Bar
Implement a `std::formatter` specialization for the progress bar class, which supports the following formatting specification:
`:<width><fill><direction><end><arrow>`, where all of the definitions are optional. The specification work as follows:
 - width of the progress bar including the bounding character (minimum 3, default 10)
 - fill describes the filling of the bar (optionas are '#', '=' or '*'; default '#')
 - direction describes whether the bars goes from the left to the right or vise verse (options are 'l2r' or 'r2l'; default 'l2r')
 - end describes the bounding symbols (options are either opening or closing braces of type '(', '[', or '|'; default '|'))
 - arrow indicates wether the last filling character should be replaced by an arrow '>'/'<' depending on the direction (options are either arrow symbol for activating; default deactivated)

The formatting result should print the bordering characters and `floor(progress * (width - 2))` of the characters in between as filled.
Example:
```
std::format("{:}", ProgressBar(1.0))      // "|########|"
std::format("{:l2r(<}", ProgressBar(1.0)) // "(#######>)"
```
> For more examples refer to the tests

For both cases you do not have to worry about invalid format specification inputs!
