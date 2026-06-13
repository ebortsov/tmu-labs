# Sheet 09 - Memory Management 2 Allocator Boogaloo

## Bump Allocator

A bump allocator trades off the ability to deallocate individual regions for extremely fast allocations. This is in fact
the strategy used for the program stack &ndash; we keep a pointer to the top and an allocation simply _bumps_
the pointer for the requested size. The only way to actually deallocate the memory is to completely clear the allocator
and move back the pointer to its starting position, which, while inflexible, is also very fast.

In this task you will implement a bump allocator that fits the C++
[`Allocator` requirement](https://en.cppreference.com/w/cpp/named_req/Allocator) and will be usable
with the STL containers like `std::vector`. There are two components required &ndash; `RawBumpAllocator` that implements
a bump allocator for a segment of raw bytes, and a template class `BumpAllocator<T>` that references a
`RawBumpAllocator` and provides the standard C++ API for allocating items of type `T`.

Multiple allocators may share the same `RawBumpAllocator` by design. We don't worry about thread safety yet.

The `RawBumpAllocator` itself can be constructed from a contiguous span of memory. It defines three public methods:

- `std::span<std::byte> allocation_span() const noexcept` that returns the assigned allocation span,
- `size_t allocated_bytes() const noexcept` that returns how many bytes of the assigned span are allocated by this
  allocator, including any padding; and
- `void clear()` that deallocates all memory allocated by this allocator.

The `BumpAllocator<T>` needs to satisfy the `Allocator` requirement and additionally define:

- `explicit BumpAllocator(RawBumpAllocator *raw)` that creates a new instance wrapping over a raw allocator,
- `RawBumpAllocator *raw_allocator() const noexcept` that returns the underlying `RawBumpAllocator`,
- `size_t remaining_size() const noexcept` which returns the theoretical maximum number of elements of type `T` that
  this allocator could allocate given the current state of the inner raw allocator; note that this is different from
  the `size_t max_size()` required by `Allocator`.

### Allocation strategy

For performance reasons, the allocator should always bump _downwards_, i.e. when working on an allocation span
`[start, end)` it should initialise its inner pointer to `end` and then move it "down" towards the `start` pointer when
fulfilling allocation requests. As you well know, allocations have to be _aligned_ properly depending on the type of
elements. The allocator should always introduce the minimum amount of padding to fulfill its requests.

For example, assume that the allocator works on the range of addresses `[0x1000000, 0x2000000)`. At first its pointer
is `0x2000000`, the top of the heap. We then request $9999$ 32-bit integers to be allocated, which requires $39996$
bytes aligned at a $4$-byte boundary (`0x9C3C` in hex). The allocator bumps its pointer to `0x1FF63C4` and returns
that as the allocation. Now a request for $16$ 64-bit integers comes, which requires $128$ bytes aligned at an $8$-byte
boundary. The allocator bumps the pointer to `0x1FF6344`, however that address is not properly aligned. Therefore,
$4$ bytes of padding are introduced and the result of the allocation is `0x1FF6340`.

### Exceptions and assertions

If a request cannot be fulfilled due to lack of available space, an `std::bad_alloc` exception must be thrown.

## Small Vector

Small-structures implementations are useful in high-performance scenarios
when creating many small instances of a given structure is common.
In the abstract the idea is that a fixed small portion of data can be held
inline within the structure without causing any heap allocations. Only when
the size exceeds that threshold is a regular dynamic allocation fallback used.
Specifically, **all elements are moved to the heap** once the small-size threshold is exceeded.
This makes sure the vector is always contiguous in memory.

In this task you will implement such an optimisation for vectors.
A `SmallVector<T, N, Allocator>` holds `N` elements of type `T` inline within its structure
footprint. Only when more than `N` elements are pushed into the vector does it
allocate heap memory using the `Allocator` allocator, where the elements in the excess of `N` are placed.

You can assume that `Allocator` is default-constructible, copy-constructible, and copy-assignable.

We will mimic the API of `std::vector`.

### Member types

- `value_type` is `T`.
- `allocator_type` is `Allocator`.
- `iterator`, the non-const iterator satisfying `std::contiguous_iterator` for elements of type `T`.
- `const_iterator`, the const iterator satisfying `std::contiguous_iterator` for elements of type `const T`.

Note that while the requirements up to `std::random_access_iterator` are mostly enforced via static concepts,
`std::contiguous_iterator` is a bit special. Firstly, you need to declare the member type
`using iterator_category = std::contiguous_iterator_tag;` on the iterators. The actual contiguity is then enforced
via a test &ndash; address of `iter + n` must be the same as `vec->data() + n`.

### Constructors

- Default constructor creates an empty vector.
- Copy constructor copies all elements from the source.
  It must allocate only as much heap memory as required to hold all elements.
- Move constructor moves all elements from the inline part of the source
  and takes ownership over its heap memory, if any. This must not allocate.
- `SmallVector(const Allocator& alloc)` creates an empty vector with a given allocator.
- `SmallVector(size_t count, const Allocator &alloc = Allocator{})` creates a vector with `count` default-initialized
  elements (and a given allocator). It should allocate the minimum amount of memory to hold all elements.
- `SmallVector(size_t count, const T &value, const Allocator &alloc = Allocator{})` creates a vector with
  `count` copies of `value` (and a given allocator). It should allocate the minimum amount of memory to hold all
  elements.
- `SmallVector(std::initializer_list<T> init, const Allocator &alloc = Allocator{})` creates a vector with
  the values given in the initializer list (and a given allocator).
  It should allocate the minimum amount of memory to hold all elements.

### Static methods

- `SmallVector with_capacity(size_t capacity, const Allocator &alloc = Allocator{})` creates an empty vector
  that can hold `capacity` elements. It should allocate the minimum amount of memory to satisfy the requirement.

### Assignment

- Copy assignment drops all elements currently in the vector and then copies all elements from the source.
  If `this` was already holding enough heap memory for all elements then it should not allocate or deallocate.
  Otherwise, it must drop its current allocation and allocate exactly as much space as required to hold all elements.
- Move assignment drops all elements currently in the vector, moves all elements from the inline part of the source,
  and then takes ownership of its heap memory, if any. This must not allocate.
- `SmallVector &operator=(std::initializer_list<T> init)` drops all elements currently in the vector and then
  copies all elements from the initializer list. Requirements on allocation are the same as in the copy-assignment
  above.

### Element access

- `T &at(size_t idx)` and `const T &at(size_t idx) const` accesses the element at index `idx` with bounds checking.
  If `idx` is out-of-bounds an `std::out_of_range` exception must be thrown.
- `T &operator[](size_t idx) noexcept` and `const T &operator[](size_t idx) const noexcept`
  accesses the element at index `idx` without bounds checking. Accessing `idx` out of bounds is undefined behaviour.
- `T &front()` and `const T &front() const` accesses the first element. Undefined behaviour for an empty vector.
- `T &back()` and `const T &back() const` accesses the last element. Undefined behaviour for an empty vector.
- `T *data()` and `const T*data() const` returns the pointer to the beginning of the vector's storage.

### Iterators

- `iterator begin()`, `const_iterator begin() const` with the obvious semantics.
- `iterator end()`, `const_iterator end() const` with the obvious semantics.

### Capacity

- `bool empty() const`, `size_t size() const` with the obvious semantics.
- `size_t capacity() const` returning the maximum number of elements that can be held in the vector without causing
  additional heap allocations.
- `bool is_inline() const` returns `true` if the vector uses the inline storage, and `false` if it uses
  a heap allocation.
- `void reserve(size_t count)` forces the vector to reserve enough memory to hold at least `count` elements.
  If it can be done with the current state of the vector, this should be a noop. Otherwise, the minimal amount of memory
  to satisfy the request should be allocated.
- `void shrink_to_fit()` force the vector to reduce its storage to the minimum possible to still hold all of its
  elements. In particular, it must move to the inline storage if possible,
- and otherwise reduce its heap allocation to minimum.

### Modifiers

- `void clear()` removes all elements from the vector **without affecting its allocation**.
- `void push_back(const T &value)` and `void push_back(T &&value)` add an element at the end of the vector by
  copying or moving it, respectively. If the current capacity is not sufficient to accommodate a new element, the vector
  should reallocate to _twice its current capacity_. See the Reallocation section below for details.
- `T &emplace_back` behaving like the `std::vector` `emplace_back`, i.e. forwarding its arguments to the constructor
  of `T` and creating the value in-place at the end of the vector. If capacity is not sufficient, reallocate as above.
- `void pop_back()` removes the last element **without affecting the vector's allocation**. Behaviour is undefined
  for an empty vector.

### Comparison

- Equality binary operators: `==`, `!=`. They should compare vectors element-wise, i.e. `vec1 == vec2` iff their size
is equal and `vec1[i] == vec2[i]` for all relevant `i`.

### Reallocation semantics

The vector should grow into the heap only when necessary, i.e. the number of elements exceeds `N`. If given a specific
reservations size with `reserve`, `with_capacity`, or by copying from another vector, it should always allocate the
minimum required memory. When growing under normal usage with `push_back` or `emplace_back` it should always allocate
twice its current capacity. In particular, when moving from inline storage to the heap the size of the first allocation
should be `2N`.

During reallocation when the vector grows your implementation should move the old elements to the new storage,
if `T` is move constructible. If not, it should use the copy constructor
(if neither is available then an implementation is impossible).

Your implementation should be in `small_vector.hpp`. Since the entire class is templated, the bulk of the
implementation will be specified inline in the header. You may use `small_vector.cpp` for implementation details, or
you may leave it empty.