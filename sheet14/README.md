# Sheet 14 - Parallelism

## InsertOnlyBag
In many multi-threaded cases there is a bunch of work, that needs to be collected with minimal synchronization overhead but the order of elements is not necessarily too important. This make for a great usecases of an insert-only bag. For this task, it is on you to implement such a thing.

It's implementation comes down to a single-linked list, where new elements are always inserted at the _beginning_ of it. Making clever use of atomics allows this to happen fully thread-safe.

The `insertonlybag.hpp` file contains the scaffolding for the implementation and define the interface of the class:
- `begin` and `end` for iterating the current state of the bag
- `insert` and `emplace` to put a new element into the bag
- a `Node` subclass for the individual element of the list

Implement a thread-safe version for all the methods only making use of atomics! Furthermore, the size of a `Node` must not exceed `sizeof(T) + sizeof(void*)` (except for alignment requirements, e.g.: the size of a integer Node might be 16 bytes) and the size of `InsertOnlyBag` should be exactly `sizeof(void*)`!

## Parallel Multimap

In this exercise you will add multi-threading support to a given data structure. The project scaffold for this exercise already contains a working _multimap_ which is implemented based on a chaining hash table (which you can't modify). Your solution should derive a thread-safe _parallel-multimap_ from the given implementation.

### Data Structure
While you do not have to implement the data structure yourself, an understanding of its inner workings is required to add proper multi-threading support. A multimap differs from a regular hash table in that it allows multiple entries with the same key. In order to keep things simple, our implementation supports only 32-bit integers as keys and values.

Internally, our multimap maintains a vector of buckets in the same way as a regular chaining hash table. Each bucket contains a pointer to the beginning of a linked list which contains all `(key, value)` entries which fall into the same bucket. Since existing entries are never overwritten, we can simply insert new entries at the beginning of this linked list without having to traverse the entire chain.

The main operations of our multimap are as follows.
- `insert`: Insert a `(key, value)` pair into the hash table. The bucket index is determined based on the key, and the entry is inserted at the beginning of the respective chain.
- `findFirst`: Returns an iterator pointing to the first entry with a given `key`. If no such entry is found, returns the past-the-end iterator of the multimap.
- `findNext`: Given an iterator, returns an iterator pointing to the next entry with the same `key`. If no such entry is found, returns the past-the-end iterator of the multimap.

Furthermore, our multimap supports iteration over all entries through the standard `begin` and `end` functions.

### Requirements
Your job is to make the functions thread-safe using mutex(es). Your thread-safe version of these functions should fulfill the following requirements.

- `insert`: must be thread-safe with respect to concurrent invocations. That is, if multiple threads invoke the it in parallel, you have to ensure that the multimap deterministically contains all entries that have been inserted in parallel. The order in which entries are inserted does not have to be deterministic.
- `findFirst`: must be thread-safe with respect to concurrent insertions. That is, it must be possible that one thread invokes the `findFirst` function while another thread concurrently invokes the `insert` function. If this situation occurs, you have to ensure that the `findFirst` function does not see the multimap in an inconsistent state.

The `begin` and `end` functions, as well as the iterator operations do _not_ have to be thread-safe. Furthermore, the `findNext` function does not have to be explicitly thread-safe, as insertions can only occur at the beginning of a chain.

We implemented test cases that check the thread-safeness of the `insert` function. Depending on your system, you may have to build the tests in release mode in order to observe any synchronization issues. Furthermore, we run the thread-sanatizer to detect potential synchronisation issues.