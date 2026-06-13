# Sheet 07 - Memory Management

For both tasks, you are not allowed to use additional headers from the standard library apart from the ones included already!
Furthermore, the usage of `shared_ptr` is prohibited for this week's homework and discouraged in general. 
Instead, we encourage the usage of `unique_ptr`.

## Binary Tree

As we can now safely interact with heap memory, we can build some more dynamic data structures.
For this exercise, you will implement a binary-search tree with the following properties:
    - each node holds a value
    - all inserted values are unique
    - each node has a left and right subtree
    - a subtree might be empty
    - all values in the left subtree are smaller than the value of a node
    - all values in the right subtree are larger than the value of a node

On removal, the removed node is replaced with the smallest node of its right subtree, if it exists otherwise with the root of the left subtree.
Example 1:
```
    1
     \
      4
     / \
    3   6
       /
      5
```
after removing (4):
```
    1
     \
      5
     / \
    3   6
```
Example 2:
```
    1
     \
      4
     / \
    3   7
       /
      5
       \
        6
```
after removing (7):
```
    1
     \
      4
     / \
    3   5
         \
          6
```
> It is __only__ a boring binary search tree - no balancing required!

The `binary-tree.hpp` file scatches two classes for modeling the tree as a whole as well as the individual nodes. 
Each Node has a set value but should __not__ be move or copy constructible or assignable.
It is your task to fill in the private area of the two classes and implement the required member functions.

- The `traverse` does a pre/in/post order traversal of the tree, printing the values along the way in the form: (in-order traversal for example tree (1)): (1)(3)(4)(5)(6)
- The `insert` methods should create a new Node on the heap or use the provided one and insert it into its appropriate location in the tree.
- The `find` and `remove` methods should throw an `runtime_error` with the message _"unable to find element"_/_"unable to remove element"_ if the requested element does not exist in the tree.
- The `size` method counts the number of contained nodes
- The `mirror` method returns a mirrored tree (revers it's in-order representation). 
The returned tree of course does no longer fullfill the sorting properties according to the binary search tree (smaller values to the left, larger ones to the right). 
This shouldn't be a concern, as the mirrored tree is only ever used for determining its size and traversing it - no further find, insert or remove calls are going to follow.
- An empty Tree should be constructible without any arguments

The address sanatizer checks for leaks at the end of your program, therefore be careful with you allocations and make sure that everything is freed properly! 
We encourage the use of `unique_ptr` over raw pointer management with `new`/`delete` for this purpose!


## Our-Template-Library Array

With the knowledge of templates, we can reimplement some parts of the standard-template-library as our-template-libarary now. We will start with a very simplified version of the `std::array` (https://en.cppreference.com/w/cpp/container/array) - a C++-like wrapper around plain old C arrays.

We want to implement a class template called `array`, which can be specialized on the type of the contained values and the number of contained elements. 
Similar to the one from the standard library, it should define the following member types:
    - `value_type`
    - `size_type`
    - `reference`
    - `const_reference`

As we are still missing the concept of how iterators work, we will restrict our implementation to the following member functions:
 - `size()`: returns number of elements
 - `operator[](index)`: returns a (const) reference to the element at that index or otherwise throws `runtime_error` with message "invalid access"_

However, it should be a bit different in how to construct them compared to the standard implementation. 
- It should be constructible from
  + no arguments - each element should be default initialized
  + one object of the element type which is copied into all elements
- It should not be move/copy assignable
- It should be copy/move constructible from another array of the same type. If the size of the other array is smaller, only those elements are copied/moved over, otherwise throw `runtime_error` with message _"too large"_.

Furthermore, we also want an optimized specialization for `bool` values, which should be stored in a bit-packed manner rather than inside an array of booleans.
In other words, we want that `sizeof(array<bool, 8>)` is one byte rather than 8 - by stuffing one boolean into every bit of a byte.

> Note: class sizes are always multiples of one byte. Therefore it is not possible to go even smaller with `sizeof(array<bool, 9>)` than two bytes.

As we can not provide a reference to a paricular bit inside a byte, we can not provide the member types `reference` and `const_reference` and also the `operator[]` returns the referenced value by-value instead of providing a reference to it.

> Note: In particular, this does no longer allow code like this `arr[0] = true;`.

Instead, we will use two alternative methods: `set(index)` and `unset(index)` to set the boolean at a particular index to `true` or `false`, again throwing `runtime_error` with message _"invalid access"_ when out-of-bounds.


For all kinds of `array` specializations, we request two further non-member functions:
 - `print(array)` which should print all values of a passed array in the format: `[1,2,3,4]\n`
 - `contains(array,elem)` which returns `true` or `false` depending on whether the array contains the provided element or not

If any part of the specification is unclear, look into the corresponding test code to find out about the intended usage.

